/*
 * Author: Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay
 * Description: This is the solution for Lab 5, Problem Statement 2.
 * Filename: main.c
 * Functions:
 * Global Variables:
 */

#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "glcd.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "Images/logo.h"

uint8_t ui8YAxisAvg;
uint8_t ui8XAxisAvg;

void main(void)
{
    _init_();
    glcd_init();
    glcd_clearDisplay();

    //Display the 8x8 square on screen, responsive to thumb stick input.
    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC1_BASE, 1);
        glcd_draw(ui8XAxisAvg, ui8YAxisAvg);
    }
}
