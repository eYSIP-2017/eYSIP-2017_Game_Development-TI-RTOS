/*

 * Author: Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Edited by: Akshay U Hegde

 * Description: This is the header file for working with JHD12864 LCD on the console.

 * Filename: glcdFunctions.h

 * Functions: glcd_cmd(), glcd_data(), glcd_init(), glcd_set_page_col(), glcd_cleardisplay(), glcd_img_write(),
              glcd_bomb_write(), glcd_digit_write(), glcd_frame2_write(), glcd_frame1_write(), glcd_frame3_write()

 * Global variables: None

 */

#include"glcdFunctions.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "glcdLibrary.h"

/*
  * Function Name: glcd_cmd()

 * Input: none

 * Output: none

 * Description: function to send commands to glcd

 * Example Call: glcd_cmd();
 */

void glcd_cmd(unsigned char cmd)
{
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0X00);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,0X00);

    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0X00);

    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,cmd);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,cmd);

    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0X01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0X00);
}


/*
  * Function Name: glcd_init()

 * Input: none

 * Output: none

 * Description: function to initialize both halfs of glcd

 * Example Call: glcd_init();
 */
void glcd_init(void)
{
    SysCtlDelay(134000);
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0X00);
    SysCtlDelay(134000);
    /* SET RST*/
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5,0X20);

    //CS=1 CS2=0
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0X00);
    //GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0X08);


    glcd_cmd(0xC0);
    glcd_cmd(0xB8);
    glcd_cmd(0x40);
    glcd_cmd(0x3F);
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0X08);

    glcd_cmd(0xC0);
    glcd_cmd(0xB8);
    glcd_cmd(0x40);
    glcd_cmd(0x3F);

}

/*
  * Function Name: glcd_data()

 * Input: none

 * Output: none

 * Description: function to send data to glcd

 * Example Call: glcd_data();
 */
void glcd_data(char data)
{
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0X00);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,0X00);

    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0X40);

    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,data);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,data);

    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0X01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0X00);
}

/*
  * Function Name: glcd_set_page_col()

 * Input: page, col

 * Output: none

 * Description: Function to select page and column number

 * Example Call: glcd_set_page_col(1,3);
 */


void glcd_set_page_col( int page,int col)
{
    if(col<64)
    {
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0X00);
        glcd_cmd(0xB8+page);
        glcd_cmd(0x40+col);

    }
    else
    {

        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,0X08);
        glcd_cmd(0xB8+page);
        glcd_cmd(0x40+(col-64));
    }
}

/*
  * Function Name: glcd_cleardisplay()

 * Input: none

 * Output: none

 * Description: function to clear display

 * Example Call: glcd_cleardisplay();
 */

void glcd_cleardisplay(void)
{
    int i,j;
    for(i=0;i<8;i++)
    {
        for(j=0;j<128;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(0x00);
        }
    }
}

/*
  * Function Name: glcd_img_write()

 * Input: none

 * Output: none

 * Description: function to write image on glcd

 * Example Call: glcd_img_write();
 */

void glcd_img_write(unsigned char img[])
{
    int i,j,k=0;
    for(i=0;i<2;i++)
    {
        for(j=0;j<128;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(img[k]);
            k++;
        }
    }
}


/*
  * Function Name: glcd_bomb_write()

 * Input: none

 * Output: none

 * Description: Function to display bomb on GLCD.

 * Example Call: glcd_bomb_write();
 */
void glcd_bomb_write()
{
    int i,j,k=78;
    for(i=0;i<8;i++)
    {
      // Display Bomb
        for(j=78;j<128;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(bomb[k]);
            k++;
        }
        k+=78;

    }

}


/*
  * Function Name: glcd_digit_write()

 * Input: number, position

 * Output: none

 * Description: Function to display the 4 digit timer

 * Example Call: glcd_digit_write(0,3);
 */

void glcd_digit_write(int number,int position)
{
   int i,j,k;
    switch (position)
    {
    case 0:
        k=0;
        for(i=0;i<2;i++)
        {
            for(j=0;j<16;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(digit[number][k]);
                k++;
            }
        }
        break;
    case 1:
        k=0;
        for(i=0;i<2;i++)
        {
            for(j=16;j<32;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(digit[number][k]);
                k++;
            }
        }
        break;
    case 2:
        k=0;
        for(i=0;i<2;i++)
        {
            for(j=38;j<54;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(digit[number][k]);
                k++;
            }
        }
        break;
    case 3:
        k=0;
        for(i=0;i<2;i++)
        {
            for(j=54;j<70;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(digit[number][k]);
                k++;
            }
        }
        break;
    }
}


/*
  * Function Name: glcd_frame2_write()

 * Input: none

 * Output: none

 * Description: Function to display frame2 of rocket.

 * Example Call: glcd_frame2_write();
 */
void glcd_frame2_write(void)
{
    int i,j,k=0;
    {
        for(i=3;i<7;i++)
        {
            for(j=72;j<128;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(frame2[k]);
                k++;
            }
        }
    }

}
/*
  * Function Name: glcd_frame1_write()

 * Input: none

 * Output: none

 * Description: Function to display frame1 of rocket.

 * Example Call: glcd_frame1_write();
 */
void glcd_frame1_write(void)
{
    int i,j,k=0,p=4;
    while(p>(-1))
    {
        k=0;
    for(i=p;i<p+4;i++)
    {
        for(j=48;j<80;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(rocket[k]);
            k++;
        }


    }
    SysCtlDelay(3000000);
    for(i=p;i<p+4;i++)
    {
        for(j=48;j<80;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(0x00);
        }
    }
    SysCtlDelay(100);
    p--;
    //glcd_cleardisplay();
    }
    k=0;
    glcd_cleardisplay();
    SysCtlDelay(100000);
    for (i=3;i<5;i++)
    {
        for(j=24;j<104;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(enter_string[k]);
            k++;
        }
    }


}
/*
  * Function Name: glcd_frame3_write()

 * Input: none

 * Output: none

 * Description: Function to display frame3 of rocket

 * Example Call: glcd_frame3_write();
 */

void glcd_frame3_write(void)
{
    int i,j,k=0;
    {
        for(i=3;i<6;i++)
        {
            for(j=6;j<54;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(frame3[k]);
                k++;
            }
        }
    }

}
/*
  * Function Name: glcd_clear_section_glcd()

 * Input: a,b,c

 * Output: none

 * Description: Fucnction to clear the section of glcd.

 * Example Call: glcd_clear_section_glcd(1,10,100)
 */
void clear_section_glcd(int a, int b,int c)
{
    int i,j;
    for(i=a;i<8;i++)
    {
        for(j=b;j<c;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(0x00);
        }
    }
}
