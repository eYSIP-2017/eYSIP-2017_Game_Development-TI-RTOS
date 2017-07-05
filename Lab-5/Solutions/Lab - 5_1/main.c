/*
 * Author:  Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay
 * Description: This is the solution for Lab 5 Problem Statement 1.
 * Filename: main.c
 * Functions: main()
 * Global Variables: None
 */

#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "glcd.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "Images/logo.h"

void main(void)
{
    _init_();
    glcd_init();
    glcd_clearDisplay();

    //Display Logo
    while(1)
    {
        glcd_write(logo);
    }
}
