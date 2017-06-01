/*
 * Copyright (c) 2015-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>



#include <xdc/runtime/Log.h>				//needed for any Log_info() call
#include <xdc/cfg/global.h> 				//header file for statically defined objects/handles
#include <xdc/runtime/Log.h>				//needed for any Log_info() call


/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
/*
 * In this program 8 images are displayed on GLCD sequentially.
 */


/* Include header files for adc and GPIO functions */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
//#include "inc/hw_gpio.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"



#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <time.h>

#include <inc/hw_gpio.h>

#include "driverlib/uart.h"



/* These header files contain the hex values of images which are stored in an unsigned char array.
 * The name of the header file corresponds the the array name in that file.
 * Eg. In one.h array is unsigned char one[]={}
      In two.h array is unsigned chaar two[]={}
      and so on till eight.h
 */


#include "one.h"
#include "two.h"
#include "three.h"
#include "four.h"
#include "five.h"
#include "six.h"
#include "seven.h"
#include "eight.h"



//Timer 2 setup
void timer2Init()
{
    uint32_t ui32Period;
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);			// enable Timer 2 periph clks
    TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);		// cfg Timer 2 mode - periodic

    /****Q. Find the period for Timer interrupt.
     *
     */
    ui32Period = (SysCtlClockGet()/500) / 2;
    TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period-1);			// set Timer 2 period

    TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);		// enables Timer 2 to interrupt CPU

    TimerEnable(TIMER2_BASE, TIMER_A);						// enable Timer 2

}




/* Variables to check the status of switch press.
 * For variables correspond to 4 switches
 * led1 - PF4
 * led2 - PD7
 * led3 - PB2
 * led4 - PF3
 */

uint8_t led1;
uint8_t led2;
uint8_t led3;
uint8_t led4;
uint8_t buzzer;
unsigned long int  delay;
volatile uint32_t tickCount=0;

/* void glcd_cmd(cmd)
 * This function sends commands to the GLCD.
 * Value of RS is 0
 * Command is written on data lines.
 * Enable is made 1 for a short duration.
 */
void glcd_cmd(unsigned char cmd)
{
    /* Clear data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 0 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x00);

    /* Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,cmd);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,cmd);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(134);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);

}



/* void glcd_data(data)
 * This function sends data to the GLCD.
 * Value of RS is 1
 * Data is written on data lines.
 * Enable is made 1 for a short duration.
 */
void glcd_data(unsigned char data)
{
    /*Clear the data lines */

    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x00);

    /* RS = 1 */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6,0x40);

    /* Put data on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,data);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,data);

    /* Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x01);
    SysCtlDelay(134);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0,0x00);
}




/* void glcd_init()
 * This function initializes the GLCD.
 * Always call this function at the beginning of main program after configuring the port pins.
 */
void glcd_init()
{
    SysCtlDelay(134000);
    /* Clear RST */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x00);
    SysCtlDelay(13400);

    /* Set RST */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0x20);

    /* Initialize left side of GLCD */
    /* Set CS1 (CS1=1 and CS2=0) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);

    /* Select the start line */
    glcd_cmd(0xC0);
    /* Send the page */
    glcd_cmd(0xB8);
    /*Send the column */
    glcd_cmd(0x40);
    /* Send glcd on command */
    glcd_cmd(0x3F);

    /* Initialize the right side of GLCD */
    /* Set CS2 (CS2=1 and CS1=0) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);

    /* Select the start line */
    glcd_cmd(0xC0);
    /* Send the page */
    glcd_cmd(0xB8);
    /*Send the column */
    glcd_cmd(0x40);
    /* Send glcd on command */
    glcd_cmd(0x3F);
}



/* void glcd_setpage(page)
 * This function selects page number on GLCD.
 * Depending on the value of column number CS1 or CS2 is made high.
 */
void glcd_setpage(unsigned char page)
{
    /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);

    /*Select the page */
    glcd_cmd(0xB8 | page);
    //  SysCtlDelay(100);

    /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);

    /*Select the page */
    glcd_cmd(0xB8 | page);
    //	SysCtlDelay(100);
}



/* void glcd_setcolumn(column)
 * This function selects column number on GLCD.
 * Depending on the value of column number CS1 or CS2 is made high.
 */
void glcd_setcolumn(unsigned char column)
{
    if(column < 64)
    {
        /* Set CS1 (CS1=1 and CS2=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x00);

        /*Select column on left side */
        glcd_cmd(0x40 | column);
        //	SysCtlDelay(6700);
    }
    else
    {
        /* Set CS2 (CS2=1 and CS1=0) The right side is selected(column>64) */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0x08);

        /*Select column on right side */
        glcd_cmd(0x40 | (column-64));
        //	SysCtlDelay(6700);
    }

}



/* void glcd_cleardisplay()
 * This function clears the data on GLCD by writing 0 on all pixels.
 */
void glcd_cleardisplay()
{
    unsigned char i,j;
    for(i=0;i<8;i++)
    {
        glcd_setpage(i);
        for(j=0;j<128;j++)
        {
            glcd_setcolumn(j);
            glcd_data(0x00);
        }
    }
}

//##########################################################################################################


/*
 *  ======== main ========
 */
int main()
{

    /* Set up the System Clock */
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    /* Enable all the peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    /* Enable the ADC */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);

    /* Configure and enable the ADC */
    ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE,3,0,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 3);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);

    /* Unlock pin PF0 */
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK)= 0;

    /* Configure Enable pin as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_DIR_MODE_OUT);

    /* Configure PE5 (RST), PE0 to PE3 (D0 to D3) and PB4 to PB7(D4 to D7) as output pins */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4 | GPIO_PIN_5| GPIO_PIN_6 | GPIO_PIN_7);

    /* Configure RS as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE,GPIO_PIN_6);

    /* Configure CS1 or CS2 as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_3);

    /* Initialize GLCD */
    glcd_init();
    SysCtlDelay(6700);
    glcd_cleardisplay();
    SysCtlDelay(9);

    /* Configure Buzzer pin as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_4,GPIO_DIR_MODE_OUT);

    /* Generate a high output on buzzer to turn it off */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4,0x10);

    /* Configure joystick button */
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);



    /* Configure the pins connected to LED as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);

    /* Unlock PD7 */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7);
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK)= 7;

    /* Configure the pins connected to push buttons as inout */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_6|GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);


    /* Turn OFF all the LEDs */
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,1);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,64);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,4);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,8);

    timer2Init();

    BIOS_start();    /* Does not return */
    return(0);
}


//This function creates the cheetahs animation by writing and clearing binary data on GLCD buffer
void updateGLCD()
{
    int i,j,p;

    while(1)
    {
        /***Semaphore_pend(*sem, wait/timeout) decrements the semaphore by 1.
         *  Until semaphore value is zero task is blocked.
         *
         */
        Semaphore_pend(GLCDsem, BIOS_WAIT_FOREVER);

        /* Get access to resource */
        j=0;p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(one[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }

        SysCtlDelay(delay/10);


        glcd_cleardisplay();

        j=0;p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(two[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }

        SysCtlDelay(delay/10);

        glcd_cleardisplay();
        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(three[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }

        SysCtlDelay(delay/10);
        //Semaphore_pend(GLCDsem, BIOS_WAIT_FOREVER);

        glcd_cleardisplay();
        // SysCtlDelay(10000);
        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(four[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }
        SysCtlDelay(delay/10);

        glcd_cleardisplay();

        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(five[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }
        SysCtlDelay(delay/10);

        glcd_cleardisplay();

        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(six[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }
        SysCtlDelay(delay/10);

        glcd_cleardisplay();

        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(seven[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }
        SysCtlDelay(delay/10);

        glcd_cleardisplay();

        j=0;
        p=0;
        while(p<8)
        {
            /* Set the page */
            glcd_setpage(p);

            for(i=0;i<128;i++)
            {
                /*Select columns from 0 to 127 */
                glcd_setcolumn(i);

                /*Send the hex values to GLCD */
                glcd_data(eight[j]);
                j++;
            }

            /*Increment the page number after previous page is filled */
            p++;

        }
        SysCtlDelay(delay/10);

        glcd_cleardisplay();

    }

}

//This function for reading ADC value
//Analog value from ADC is used to change the delay between frames in updateGLCD
//by varying 'delay'.
void readADC()
{
    uint32_t ui32ADC0Value,y;    // Variable to store the value of digital data after conversion

    while(1)
    {

        /***Semaphore_pend(*sem, wait/timeout) decrements the semaphore by 1.
         *  Until semaphore value is zero task is blocked.
         *
         */
        Semaphore_pend(ADCsem, BIOS_WAIT_FOREVER);

        /* Get access to resource */
        /* Clear the ADC interrupt flag and start the conversion process */
        ADCIntClear(ADC1_BASE, 3);
        ADCProcessorTrigger(ADC1_BASE, 3);

        /* Wait till conversion is complete */
        while(!ADCIntStatus(ADC1_BASE, 3, false))
        {
        }

        /* Clear the ADC interrupt flag and get the conversion result */
        ADCIntClear(ADC1_BASE, 3);
        ADCSequenceDataGet(ADC1_BASE, 3,&ui32ADC0Value);

        /*Divide the result of ADC to get delay */
        y =  ui32ADC0Value;
        if(y<255)
        {

            delay=6700000;
        }
        else if(y>4000)
        {

            delay=670000;
        }
        else
        {

            delay=670000*2;
        }



    }

}

//This function reads switch and updates corresponding LEDs.
void readSwitch()
{
    while(1)
    {

        /***Semaphore_pend(*sem, wait/timeout) decrements the semaphore by 1.
         *  Until semaphore value is zero task is blocked.
         *
         */
        Semaphore_pend(SWITCHsem, BIOS_WAIT_FOREVER);

        /* Get access to resource */
        /* Check the status of switch 1 */
        led1=GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4);

        /* If switch is pressed turn ON the corresponding LED */
        if(led1 ==0 )
        {
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,2);
        }

        /* Check the status of switch 2 */
        led2=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_7);

        /* If switch is pressed turn ON the corresponding LED */
        if(led2 ==0 )
        {
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,32);
        }

        /* Check the status of switch 3 */
        led3=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_6);

        /* If switch is pressed turn ON the corresponding LED */
        if(led3==0)
        {
            GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,4);
        }

        /* Check the status of switch 4 */
        led4=GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_7);

        /* If switch is pressed turn ON the corresponding LED */
        if(led4==0 )
        {
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,0);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,8);
        }

        buzzer=GPIOPinRead(GPIO_PORTE_BASE,GPIO_PIN_4);

        if(buzzer==0)
        {
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4,0x00);
        }
        else
        {
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4,0x10);
        }

    }
}



//---------------------------------------------------------------------------
// Timer ISR  to be called by BIOS Hwi
//
// Posts Semaphore for releasing tasks
//---------------------------------------------------------------------------
void Timer_ISR(void)
{
    TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);			// must clear timer flag FROM timer
    tickCount++;  //tickCount is incremented every 2 ms.


    /******Write code for releasing 3 tasks in periodic manner
     *
     *    For running the task, semaphore corresponding to Task has to be posted.
     *    For eg. Semaphore_post(ADCsem); ADCsem is incremented by one, which unblocks the
     *    Task.
     *
     */
    if(tickCount == 10)
    {
        Semaphore_post(GLCDsem);
    }
    else if(tickCount == 20)
    {
        Semaphore_post(ADCsem);
    }
    else if(tickCount == 30)
    {
        Semaphore_post(SWITCHsem);
        tickCount = 0;
    }
}



