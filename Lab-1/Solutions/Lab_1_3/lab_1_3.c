/*

 * Author: Texas Instruments

 * Edited by: Saurav Shandilya, Vishwanathan Iyer, Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 1, Problem Statement 3.

 * Filename: lab_1_3.c

 * Functions: setup(), ledPinConfig(), switchPinConfig(), chengeDelay(), changeColor(), toggleLED(), main()

 * Global Variables: LEDcount, delay

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
    uint8_t LEDcount = 2;
    uint32_t delay = 6700000;
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
    GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-0 and Pin-4 of PORT F as Input.
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}
/*

 * Function Name: changeDelay()

 * Input: none

 * Output: none

 * Description: Whenever SW1 is pressed, switch between 0.5,1 and 2 ms delay.

 * Example Call: changeDelay();

 */
void changeDelay(void)
{
    //Change Toggle Time
    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) == 0x00) //Check SW1 ON
    {
        if(delay < 26800000)
            delay *= 2;
        else
            delay = 6700000;
    }
}
/*

 * Function Name: changeColor()

 * Input: none

 * Output: none

 * Description: Whenever SW2 is pressed, switch between R,G and B colors.

 * Example Call: changeColor();

 */
void changeColor(void)
{
    if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) == 0x00) //Check SW2 ON
    {
        if(LEDcount < 8) //Toggle LED Color
            LEDcount *= 2;
        else
            LEDcount = 2;
    }
}
/*

 * Function Name: toggleLED()

 * Input: none

 * Output: none

 * Description: Toggles LED ON and OFF for given color and delay.

 * Example Call: toggleLED();

 */
void toggleLED(void)
{
    // Turn on the LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, LEDcount);
    // Delay for a bit
    SysCtlDelay(delay);
    // Check Switches
    changeColor();
    changeDelay();
    //Turn off the LED
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
    //Delay for a bit
    SysCtlDelay(delay);
    // Check Switches
    changeColor();
    changeDelay();
}

int main(void)
{
    //System Configuration Function Call
    setup();
    ledPinConfig();
    switchPinConfig();

    while(1)
    {
        // Main LED Toggle Function Call
        toggleLED();
    }
}
