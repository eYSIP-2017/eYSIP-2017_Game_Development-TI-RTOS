/*

 * Author: Texas Instruments

 * Edited by: Saurav Shandilya, Vishwanathan Iyer, Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 1, Problem Statement 2.

 * Filename: lab_1_2.c

 * Functions: setup(), ledPinConfig(), switchPinConfig(), main()

 * Global Variables: sw2Status

 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
 */
uint32_t sw2Status = 0;
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

 * Function Name: ledPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

 * Example Call: ledPinConfig();

 */

void ledPinConfig(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1, Pin-2 and Pin-3 of PORT F set as output.
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */
void switchPinConfig(void)
{
    // Following two lines remove the lock from SW2 interfaced on PORTF Pin0
    LOCK_F=0x4C4F434BU;
    CR_F=GPIO_PIN_0|GPIO_PIN_4;

    // GPIO PORTF Pin 0 and Pin4
    GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-0 of PORT F as Input. Modifiy this to use another switch
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}


int main(void)
{
    //System Configuration Function Call
    setup();
    ledPinConfig();
    switchPinConfig();

    while(1){

        if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) == 0x00 ) //Check SW2 ON
        {
            sw2Status++; //Increment count
            SysCtlDelay(4000000); //Delay to avoid multiple increment
        }

    }
}
