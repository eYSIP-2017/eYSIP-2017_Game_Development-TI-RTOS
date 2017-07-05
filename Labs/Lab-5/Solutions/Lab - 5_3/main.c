/*

 * Author: Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 5 Problem Statement 3

 * Filename: main.c

 * Functions: main()

 * Global Variables: ui8YAxisAvg, ui8XAxisAvg

 */

#include <stdint.h>
#include <stdbool.h>
#include "console.h"
#include "glcd.h"
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "Animation/cheetah.h"
/*
 * Global Variable Declaration
 */
uint8_t ui8YAxisAvg;
uint8_t ui8XAxisAvg;
uint32_t latency;

void main(void)
{
    unsigned char index;
    latency = 5;

    _init_();
    glcd_init();
    glcd_clearDisplay();

    //Display Cheetah Animation
    while(1)
    {
        for(index = 0; index < 8; index++) // Loop through each frame
        {
            unsigned char i = 0, p = 0;
            uint32_t j = 0;
            while(p < 8) // Loop through each poge
            {
                // Set the page
                glcd_setPage(p);

                for(i = 0; i < 128; i++) //Select each column
                {
                    // Select all the columns
                    glcd_setColumn(i);

                    //Send hex values GLCD
                    glcd_data((cheetah[index][j])); // Input Data from hex
                    j++;
                }
                p++;
            }

            // Processor Trigger ADC Conversion

            ADCProcessorTrigger(ADC0_BASE, 1);
            ADCProcessorTrigger(ADC1_BASE, 1);

            // Speed change criteria
            if (ui8XAxisAvg < 20)
                latency = 12;
            else if (ui8XAxisAvg > 110)
                latency = 2;
            else
                latency = 5;
        }
    }
}
