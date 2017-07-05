/*
 * Author: Texas Instruments

 * Edited by: Saurav Shandilya, Vishwanathan Iyer, Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 2.

 * Filename: lab_2_1.c

 * Functions: setup(), ledPinConfig(), timerConfig(), interruptConfig(), switchPinConfig(), changeColor(), detectKeyPress(), Timer0IntHandler(), main()

 * Global Variables: ui32Period, LEDcount, pinName, sw2State, flag

 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
 */
uint32_t ui32Period;
uint8_t LEDcount = 2;
uint32_t pinName;
uint32_t sw2State;
unsigned int flag = 0;
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

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

 * Function Name: timerConfig()

 * Input: none

 * Output: none

 * Description: Set Timer0 as 32 bit timer in Periodic mode. And set GPIO toggle at 10 Hz and 50% duty cycle.

 * Example Call: timerConfig();

 */

void timerConfig(void)
{
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (SysCtlClockGet() / 20) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
}
/*

 * Function Name: interruptConfig()

 * Input: none

 * Output: none

 * Description: Enable Interrupts and Timers.

 * Example Call: interruptConfig();

 */
void interruptConfig(void)
{
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    TimerEnable(TIMER0_BASE, TIMER_A);
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 4 and Pin 0 as input. Note that Pin 0 is locked.

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

 * Function Name: changeColor()

 * Input: none

 * Output: none

 * Description: Switch between R,G and B colors.

 * Example Call: changeColor();

 */
void changeColor(void)
{
        if(LEDcount < 8) //Toggle LED Color
            LEDcount *= 2;
        else
            LEDcount = 2;
}
/*
 * State values and state initialization for State Machine
 */
enum states{

     IDLE,
     PRESS,
     RELEASE
};

enum states state = IDLE;
/*

 * Function Name: detectKeyPress()

 * Input: none

 * Output: none

 * Description: State Machine which performs Switch Debouncing, for accurate switch press detection.

 * Example Call: detectKeyPress();

 */
unsigned int detectKeyPress()
{
    switch(state){
    case IDLE:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state = PRESS;
            return 0;
        }
        else
        {
            state = IDLE;
            return 0;
        }
    case PRESS:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state = RELEASE;
            return 1;
        }
        else
        {
            state = IDLE;
            return 0;
        }
    case RELEASE:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state = RELEASE;
            return 0;
        }
        else
        {
            state = IDLE;
            return 0;
        }
    }
}
/*

 * Function Name: Timer0IntHandler()

 * Input: none

 * Output: none

 * Description: Timer 0 Interrupt Routine.

 * Example Call: Timer0IntHandler();

 */
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    pinName = GPIO_PIN_4;
    flag = detectKeyPress();
    if(flag == 1)
    {
        changeColor();
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, LEDcount);
    }
}

int main(void)
{
    //System Configuration Function Call
    setup();
    ledPinConfig();
    switchPinConfig();
    timerConfig();
    interruptConfig();

    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, LEDcount);

    while(1)
    {
    }
}
