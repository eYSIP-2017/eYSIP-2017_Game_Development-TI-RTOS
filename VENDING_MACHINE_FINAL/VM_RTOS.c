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
#include <string.h>
#include <time.h>
#include "Console/console.h"
#include "Console/glcd.h"

#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"



/* These header files contain the hex values of images which are stored in an unsigned char array.
 * The name of the header file corresponds the the array name in that file.
 * Eg. In one.h array is unsigned char one[]={}
      In two.h array is unsigned chaar two[]={}
      and so on till eight.h
 */
#include "Images/images.h"

/* Global Variables
 */

uint32_t latency;
signed int sum;
char digits[4] = "00c";
unsigned char p, ch, reg_n, diet_n, holder, character;
char text[];
unsigned char temp[8];
unsigned int flag;
uint32_t pinName, baseName;

volatile uint32_t tickCount=0;

/* void glcd_cmd(cmd)
 * This function sends commands to the GLCD.
 * Value of RS is 0
 * Command is written on data lines.
 * Enable is made 1 for a short duration.
 */
enum states{

    IDLE,
    PRESS,
    RELEASE
};

enum states state[5] = {IDLE, IDLE, IDLE, IDLE, IDLE};

enum vm_modes{
    INITM,
    COIN,
    SELECT,
    DELIVERY
};

enum vm_modes vm_mode = INITM;

enum inputs{
    INITI,
    DIME,
    NICKEL,
    QUARTER,
    NONEI
};

enum inputs input = INITI;

enum outputs{
    INITO,
    REGULAR,
    DIET,
    CHANGE,
    NONEO
};

enum outputs output = INITO;

void switchSelect(unsigned char index)
{
    switch(index){
    case 0:
        baseName = GPIO_PORTC_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 1:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 2:
        baseName = GPIO_PORTF_BASE;
        pinName = GPIO_PIN_4;
        break;
    case 3:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_6;
        break;
    case 4:
        baseName = GPIO_PORTE_BASE;
        pinName = GPIO_PIN_4;
        break;
    }
}

/*

 * Function Name: detectKeyPress()

 * Input: uint32_t pinName, unsigned int N

 * Output: unsigned int flag

 * Description: State Machine which performs Switch Debouncing, for accurate switch press detection.

 * Example Call: detectKeyPress(GPIO_PORTC_BASE, GPIO_PIN_7, 0);

 */
unsigned int detectKeyPress(unsigned char n)
{
    switchSelect(n);
    switch(state[n]){
    case IDLE:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = PRESS;
            return 0;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case PRESS:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 1;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case RELEASE:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 0;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    }
}

void COIN_detectInput()
{
    digits[1] = (sum%10) + '0';
    digits[0] = (sum/10) + '0';
    if(detectKeyPress(0) == 1)
    {
        input = DIME;
        sum += 5;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(2) == 1)
    {
        input = NICKEL;
        sum += 10;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(4) == 1)
    {
        input = QUARTER;
        sum += 25;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(1) == 1)
    {
        vm_mode = SELECT;
    }
}
void COIN_display()
{
    switch(input){
    case INITI:
        glcd_write(coin);
        displayText("  5c>Press UP    10c>Press DOWN  25c>Press HAT  Next>Press RIGHT", 4);
        input = NONEI;
        break;
    case DIME:
        displayText(digits, 0);
        displayText("Entered                         5c>Press UP     10c>Press DOWN  25c>Press HAT   Next>Press RIGHT", 1);
        break;
    case NICKEL:
        displayText(digits, 0);
        displayText("Entered                         5c>Press UP     10c>Press DOWN  25c>Press HAT   Next>Press RIGHT", 1);
        break;
    case QUARTER:
        displayText(digits, 0);
        displayText("Entered                         5c>Press UP     10c>Press DOWN  25c>Press HAT   Next>Press RIGHT", 1);
        break;
    case NONEI:
        break;
    }
}
void SELECT_detectInput()
{
    if(detectKeyPress(3) == 1)
    {
        sum -= 35;
        reg_n++;
        output = REGULAR;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(1) == 1)
    {
        sum -= 35;
        output = DIET;
        diet_n++;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(0) == 1)
    {
        output = CHANGE;
        glcd_clearDisplay();
    }
    else if(detectKeyPress(2) == 1)
    {
        vm_mode = DELIVERY;
    }
}

void SELECT_display()
{
    switch(output)
    {
    case INITO:
        glcd_clearDisplay();
        display40x32(0, 3, soda_can);
        displayText("Reg>Press LEFT  Diet>Press RIGHTCancel>Press UP", 5);
        output = NONEO;
        break;
    case REGULAR:
        displayText("Selected Regular                Reg>Press LEFT  Diet>Press RIGHTContinue>Press  DOWN", 2);
        break;
    case DIET:
        displayText("Selected Diet                   Reg>Press LEFT  Diet>Press RIGHTContinue>Press  DOWN", 2);
        break;
    case CHANGE:
        displayText("Abort Transaction                Continue>Press  DOWN", 2);
        break;
    case NONEO:
        break;
    }
}
void DELIVERY_output()
{
    switch(output){
    case REGULAR:
        if(sum >= 0)
        {
            glcd_clearDisplay();
            display40x32(1, 3, smiley);
            displayText("  Please Enjoy     Your Drink!", 5);
            while(reg_n > 0)
            {
                ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x00);
                millis(1000);
                ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x02);
                millis(1000);
                reg_n--;
            }
        }
        else
        {
            sum =+ ((diet_n+reg_n)*35);
            glcd_clearDisplay();
            displayText(" Sorry, unable to dispense. Insufficient Cash", 2);
        }
        output = CHANGE;
        break;
    case DIET:
        if(sum >= 0)
        {
            glcd_clearDisplay();
            display40x32(1, 3, smiley);
            displayText("  Please Enjoy     Your Drink!", 5);
            while(diet_n > 0)
            {
                ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x00);
                millis(1000);
                ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x08);
                millis(1000);
                diet_n--;
            }
        }
        else
        {
            sum += ((diet_n+reg_n)*35);
            glcd_clearDisplay();
            displayText(" Sorry, unable  to dispense. In-sufficient Cash", 2);
            millis(2000);
        }
        output = CHANGE;
        break;
    case CHANGE:
        glcd_clearDisplay();
        glcd_write(coin);
        displayText("    Collect         Change", 6);
        millis(2000);
        while(sum > 0)
        {
            ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x00);
            millis(500);
            ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x20);
            millis(500);
            sum -= 5;
        }
        glcd_clearDisplay();
        displayText(" Thank you for  using SodVen    Vending Machine!", 2);
        millis(2000);
        glcd_clearDisplay();
        vm_mode = INITM;
        break;
    }
}

void updateGLCD(void)
{
    while(1)
    {
        Semaphore_pend(GLCDsem, BIOS_WAIT_FOREVER);

        switch(vm_mode){
        case INITM:
            sum = 0;
            reg_n = 0;
            diet_n = 0;
            vm_mode = COIN;
            input = INITI;
            output = INITO;
        case COIN:
            COIN_display();
            break;
        case SELECT:
            SELECT_display();
            break;
        case DELIVERY:
            DELIVERY_output();
            break;
        }
    }
}

void readSwitch(void)
{
    while(1)
    {
        Semaphore_pend(SWITCHsem, BIOS_WAIT_FOREVER);

        switch(vm_mode){
        case INITM:
            break;
        case COIN:
            COIN_detectInput();
            break;
        case SELECT:
            SELECT_detectInput();
            break;
        case DELIVERY:
            break;
        }
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    latency = 40;
    _init_();
    glcd_init();
    glcd_clearDisplay();
    ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x20);
    ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0x04);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x02);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x08);

    /* Start BIOS */
    BIOS_start();

    return (0);
}

void Timer_ISR(void)
{
    ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    tickCount++;
    switch(tickCount){
    case 15:
        Semaphore_post(SWITCHsem);
        break;
    case 30:
        Semaphore_post(GLCDsem);
        tickCount = 0;
        break;
    }
}
