/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the definitions of the header file for working with JHD12864 LCD on the console.

 * Filename: glcd.c

 * Functions: glcd_data(), glcd_cmd(), glcd_setColumn(), glcd_setPage(), glcd_cleanup(), glcd_clearDisplay(),
              glcd_write, glcd_draw, display40x32(), textToGLCD(), displayText(), glcd_init()

 * Global Variables: p, block_x, x_prev, y_prev, holder, latency, pinName, baseName, temp[8], flag

 */
/*
 * Requisite Headers including console initialization functions, and custom font library.
 */
#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "Images/characters.h"

// ROM Libary
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

/*
 * Global Variable Definition
 */
unsigned char p, block_x, x_prev, y_prev, holder;
uint32_t latency;
uint32_t pinName, baseName;
unsigned char temp[8];
unsigned int flag;
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
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x00);

    ROM_GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x40);

    // Put Data on Data Lines
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, data);
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, data);

    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    micros(1);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);
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
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0x00);
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x00);

    ROM_GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0x00);

    // Put Command on Data Lines
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, cmd);
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, cmd);

    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x01);
    micros(1);
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x00);

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
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

        // Select the page
        glcd_cmd(0x40 | column);
        micros(latency);
    }
    else
    {
        // CS2 = 1, CS1 = 0
        ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

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
    ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);

    // Select the page
    glcd_cmd(0xB8 | page);
    micros(50);

    //CS2 = 1, CS1 = 0
    ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);

    // Select the page
    glcd_cmd(0xB8 | page);
    micros(50);

}
void glcd_cleanup(unsigned char page, unsigned char quadrant)
{
    unsigned char j;
    glcd_setPage(page);
    for(j = (quadrant*16); j < ((quadrant*16) + 32); j++)
    {
        glcd_setColumn(j);
        glcd_data(0x00);
    }
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
    unsigned char i;
    // Clear Previous 8x8 square
    glcd_setPage(y_prev);
    for( i = 0; i < 8 ; i++)
    {
        // Prevents Clearing out of screen
        block_x = x_prev - (4-i);
        if(x_prev < 4)
            x_prev = 4;
        else if (x_prev > 123)
            x_prev = 123;
        glcd_setColumn(block_x);
        glcd_data(0x00);
    }
    // Draw current 8x8 square
    glcd_setPage(y_pos);
    for( i = 0; i < 8 ; i++)
    {
        // Prevents square going out of screen
        if((x_pos) < 4)
            x_pos = 4;
        else if (x_pos > 123)
            x_pos = 123;
        block_x = x_pos - (4-i);
        glcd_setColumn(block_x);
        glcd_data(0xFF);
    }
    // Store value for clearing
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
    unsigned char i = 0;
    uint32_t j = 0;
    p = page;
    while(p < (page+5))
    {
        glcd_setPage(p);

        for(i = (quadrant*16); i < (quadrant*16 + 32); i++)
        {
            // Select the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(image[j]);
            j++;
        }
        p++;
    }
}
/*

 * Function Name: textToGLCD(unsigned char character)

 * Input: character

 * Output: none

 * Description: Assigns corresponding GLCD character from the custom font library to each Ascii Value.

 * Example Call: textToGLCD(a)

 */
void textToGLCD(unsigned char character)
{
    unsigned char q = 0;
    unsigned char i;
    while(q < 128)
    {
        i = 0;
        while(i < 8)
        {
            if(character == q)
            {
                // Compare ascii value with the input and assign corresponding hex values from font library.
                temp[i] = ascii[(q*8) + i];
            }
            i++;
        }
        q++;
    }
}
/*

 * Function Name: void displayText(char *text, unsigned char page)

 * Input: *text, page

 * Output: none

 * Description: Writes strings directly onto the specific page on the GLCD, starting from 0 column.

 * Example Call: displayText("Hola", 0)

 */
void displayText(char *text, unsigned char page)
{
    unsigned char i = 0, j = 0, k = 0;
    unsigned char cursor = 0;
    for(i = 0; i < strlen(text); i++)
    {
        holder = text[i];
        textToGLCD(holder);
        j = 0;
        glcd_setPage(page);
        for(k = (cursor*8); k < (cursor*8 + 8); k++)
        {
            // Select all the columns
            glcd_setColumn(k);
            //Send hex values GLCD
            glcd_data(temp[j]);
            j++;
        }
        // Text moves on to next page automatically on end of line
        if(cursor == 15)
        {
            if(page <= 7)
            {
                page ++;
            }
            else
            {
                // Display from first line again, on end of page
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

 * Function Name: glcd_init()

 * Input: none

 * Output: none

 * Description: Initializes the GLCD Screen left and right halves.

 * Example Call: glcd_init();

 */
void glcd_init(void)
{
    micros(1000);

    //Clear RST
    ROM_GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_5, 0x00);

    micros(1000);

    //Set RST
    ROM_GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x20);

    ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x00);
    // Select Start Line
    glcd_cmd(0xC0);
    // Send Page
    glcd_cmd(0xB8);
    // Send Column
    glcd_cmd(0x40);
    // Send GLCD on command
    glcd_cmd(0x3F);

    ROM_GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0x08);
    // Select Start Line
    glcd_cmd(0xC0);
    // Send Page
    glcd_cmd(0xB8);
    // Send Column
    glcd_cmd(0x40);
    // Send GLCD on command
    glcd_cmd(0x3F);
}
