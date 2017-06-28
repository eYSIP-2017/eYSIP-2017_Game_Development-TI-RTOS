/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Despande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the definitions of the header file for working with JHD12864 LCD on the console.

 * Filename: glcd.c

 * Functions: glcd_data(), glcd_cmd(), glcd_setColumn(), glcd_setPage(), glcd_cleanup(), glcd_clearDisplay(),
              glcd_clearRow(), glcd_write(), glcd_draw(), display40x32(), smallTextToGLCD(), largeTextToGLCD(), displaySmallText(),
              displayLargeText(), glcd_init()

 * Global Variables: block_x, x_prev, y_prev, holder, latency, pinName, baseName, tempSmall[8], tempLarge[32]

 */

// Standard C Header
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Custom Console Init and Font Library
#include "consoleInit.h"
#include "characterset.h"

// Header files for GPIO Functions
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

// ROM Header
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"


/*
 * Global Variable Definition
 */

unsigned char block_x, x_prev, y_prev, holder;
uint32_t latency, pinName, baseName;
volatile unsigned char tempSmall[8];
volatile unsigned char tempLarge[32];
/*

 * Function Name: glcd_data(unsigned char data)

 * Input: data

 * Output: none

 * Description: Sends data onto the LCD Data Pins in Data Mode.

 * Example Call: glcd_data(data);

 */
void glcd_data(unsigned char data)
{
    //Clear data lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x00);

    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);

    // Put Data on Data Lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, data);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, data);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    micros(1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
}
/*

 * Function Name: glcd_cmd(unsigned char cmd)

 * Input: cmd

 * Output: none

 * Description: Sends commands onto the LCD Data Pins in Command Mode.

 * Example Call: glcd_cmd(cmd);

 */
void glcd_cmd(unsigned char cmd)
{
    //Clear data lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x00);

    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x00);

    // Put Command on Data Lines
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, cmd);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, cmd);

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    micros(1);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);

}
/*

 * Function Name: glcd_setColumn(unsigned char column)

 * Input: column

 * Output: none

 * Description: Select the requisite column for output.

 * Example Call: glcd_setColumn(column);

 */
void glcd_setColumn(unsigned char column)
{
    if(column < 64)
    {
        // CS1 = 1, CS2 = 0
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

        // Select the page
        glcd_cmd(0x40 | column);
        micros(latency);
    }
    else
    {
        // CS2 = 1, CS1 = 0
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

        // Select the page
        glcd_cmd(0x40 | (column - 64));
        micros(latency);
    }
}
/*

 * Function Name: glcd_setPage(unsigned char page)

 * Input: page

 * Output: none

 * Description: Select the requisite row page for output.

 * Example Call: glcd_setPage(page);

 */
void glcd_setPage(unsigned char page)
{

    // CS1 = 1, CS2 = 0
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

    // Select the page
    glcd_cmd(0xB8 | page);
    micros(50);

    //CS2 = 1, CS1 = 0
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

    // Select the page
    glcd_cmd(0xB8 | page);
    micros(50);

}
/*

 * Function Name: glcd_clearDisplay(void)

 * Input: none

 * Output: none

 * Description: Clears the LCD Screen.

 * Example Call: glcd_clearDisplay();

 */
void glcd_clearDisplay(void)
{
    unsigned char i,j;
    for(i = 0; i < 8; i++)
    {
        glcd_setPage(i);
        for(j = 0; j < 128; j++)
        {
            glcd_setColumn(j);
            glcd_data(0x00);
        }
    }
}
/*

 * Function Name: glcd_clearDisplay(unsigned char rowClear)

 * Input: rowClear

 * Output: none

 * Description: Clears a particular page of the GLCD.

 * Example Call: glcd_clearRow(4);

 */
void glcd_clearRow(unsigned char rowClear)
{
    unsigned char j;
    glcd_setPage(rowClear);
    for(j = 0; j < 128; j++)
    {
        glcd_setColumn(j);
        glcd_data(0x00);
    }
}
/*

 * Function Name: glcd_write(unsigned char *image)

 * Input: *image

 * Output: none

 * Description: It displays the input array as an image on the screen

 * Example Call: glcd_write(array);

 */
void glcd_write(unsigned char *image)
{
    unsigned char i = 0, p = 0;
    uint32_t j = 0;
    while(p < 8)
    {
        // Set the page
        glcd_setPage(p);

        for(i = 0; i < 128; i++)
        {
            // Select all the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(image[j]);
            j++;
        }
        p++;
    }
}
/*

 * Function Name: glcd_draw(unsigned char x_pos, unsigned char y_pos)

 * Input: x_pos, y_pos

 * Output: none

 * Description: Sets a 8x8 square on the screen, at the required row,column coordinates.

 * Example Call: glcd_draw(x,y);

 */
void glcd_draw(unsigned char x_pos, unsigned char y_pos)
{
    glcd_setPage(y_prev);
    unsigned char i;
    for( i = 0; i < 24 ; i++)
    {
        block_x = x_prev - (4-i);
        if(x_prev < 4)
            x_prev = 4;
        else if (x_prev > 123)
            x_prev = 123;
        glcd_setColumn(block_x);
        glcd_data(0x00);
    }
    glcd_setPage(y_pos);
    for( i = 0; i < 24 ; i++)
    {
        if((x_pos) < 4)
            x_pos = 4;
        else if (x_pos > 123)
            x_pos = 123;
        block_x = x_pos - (4-i);
        glcd_setColumn(block_x);
        glcd_data(0xFF);
    }
    x_prev = x_pos;
    y_prev = y_pos;
}
/*

 * Function Name: display40x32(unsigned char page, unsigned char quadrant, unsigned char *image)

 * Input: page, quadrant, *image

 * Output: none

 * Description: Displays an image of size 40x32 pixels on the GLCD at the required position on the screen.

 * Example Call: display40x32(3, 2, coin)

 */
void display40x32(unsigned char page, unsigned char quadrant, unsigned char *image)
{
    unsigned char i = 0, p;
    uint32_t j = 0;
    p = page;
    while(p < (page+5))
    {
        glcd_setPage(p);

        for(i = (quadrant*16); i < (quadrant*16 + 32); i++)
        {
            // Select all the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(image[j]);
            j++;
        }
        p++;
    }
}
/*

 * Function Name: smallTextToGLCD(unsigned char character)

 * Input: character

 * Output: none

 * Description: Assigns corresponding GLCD character of font size 8 from the custom font library to each Ascii Value.

 * Example Call: smallTextToGLCD(a)

 */
void smallTextToGLCD(unsigned char character)
{
    unsigned char q = 0;
    unsigned char i;
    while(q < 128)
    {
        i = 0;
        if(character == q)
        {
            while(i < 8)
            {
                // Compare ascii value with the input and assign corresponding hex values from font library.
                tempSmall[i] = asciiSmall[(q*8) + i];
                i++;
            }
        }
        q++;
    }
}
/*

 * Function Name: largeTextToGLCD(unsigned char character)

 * Input: character

 * Output: none

 * Description: Assigns corresponding GLCD character of font size 16 from the custom font library to each Ascii Value.

 * Example Call: largeTextToGLCD(a)

 */
void largeTextToGLCD(unsigned char character)
{
    unsigned char q = 0;
    unsigned char i;
    while(q < 128)
    {
        i = 0;
        if(character == q)
        {
            while(i < 32)
            {
                // Compare ascii value with the input and assign corresponding hex values from font library.
                tempLarge[i] = asciiLarge[(q*32) + i];
                i++;
            }
        }
        q++;
    }
}
/*

 * Function Name: void displayLargeText(char *text, unsigned char page, unsigned char cursor)

 * Input: *text, page, cursor

 * Output: none

 * Description: Writes strings directly onto the specific page on the GLCD, and specific cursor position(different from the
                displayed cursor) of the font size 16.

 * Example Call: displayLargeText("Hola", 0, 3)

 */
void displayLargeText(char *text, unsigned char page, unsigned char cursor)
{
    unsigned char i = 0, j = 0, k = 0;
    for(i = 0; i < strlen(text); i++)
    {
        holder = text[i];
        largeTextToGLCD(holder);
        j = 0;
        glcd_setPage(page);
        for(k = (cursor*16); k < (cursor*16 + 16); k++)
        {
            // Select all the columns
            glcd_setColumn(k);
            //Send hex values GLCD
            glcd_data(tempLarge[j]);
            j++;
        }
        glcd_setPage((page+1));
        for(k = (cursor*16); k < (cursor*16 + 16); k++)
        {
            // Select all the columns
            glcd_setColumn(k);
            //Send hex values GLCD
            glcd_data(tempLarge[j]);
            j++;
        }
        // Text moves on to next page automatically on end of line
        if(cursor == 7)
        {
            // Display from first line again, on end of page
            if(page <= 7)
            {
                page ++;
            }
            else
            {
                page = 0;
            }
            cursor = 0;
        }
        else
        {
            cursor++;
        }
    }
}
/*

 * Function Name: void displaySmallText(char *text, unsigned char page, unsigned char cursor)

 * Input: *text, page, cursor

 * Output: none

 * Description: Writes strings directly onto the specific page on the GLCD, and specific cursor position(different from the
                displayed cursor) of the font size 8.

 * Example Call: displaySmallText("Hello", 0, 3)

 */
void displaySmallText(char *text, unsigned char textPage, unsigned char smallCursor)
{
    unsigned char i = 0, j = 0, k = 0;
    for(i = 0; i < strlen(text); i++)
    {
        holder = text[i];
        smallTextToGLCD(holder);
        j = 0;
        glcd_setPage(textPage);
        for(k = (smallCursor*8); k < (smallCursor*8 + 8); k++)
        {
            // Select all the columns
            glcd_setColumn(k);
            //Send hex values GLCD
            glcd_data(tempSmall[j]);
            j++;
        }
        // Text moves on to next page automatically on end of line
        if(smallCursor == 15)
        {
            // Display from first line again, on end of page
            if(textPage <= 7)
            {
                textPage ++;
            }
            else
            {
                textPage = 0;
            }
            smallCursor = 0;
        }
        else
        {
            smallCursor++;
        }
    }
}
/*

 * Function Name: glcd_init()

 * Input: none

 * Output: none

 * Description: Initializes the GLCD Screen.

 * Example Call: glcd_init();

 */
void glcd_init(void)
{
    micros(1000);

    //Clear RST
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5, 0x00);

    micros(1000);

    //Set RST
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x20);

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);
    // Select Start Line
    glcd_cmd(0xC0);
    // Send Page
    glcd_cmd(0xB8);
    // Send Column
    glcd_cmd(0x40);
    // Send GLCD on command
    glcd_cmd(0x3F);


    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);
    // Select Start Line
    glcd_cmd(0xC0);
    // Send Page
    glcd_cmd(0xB8);
    // Send Column
    glcd_cmd(0x40);
    // Send GLCD on command
    micros(100);
    glcd_cmd(0x3F);
    micros(100);
}
