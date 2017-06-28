/*

 * Author: Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Edited by: Akshay U Hegde

 * Description: This is the initialization definitions of the header file for the console which
                initializes and sets required peripherals.

 * Filename: setupConsole.c

 * Functions: setupConsole()

 * Global variables: ui32Period, ui32Period1

 */
#include "setupConsole.h"
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
#include <inc/hw_gpio.h>

volatile uint32_t ui32Period,ui32Period1;

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))


void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    //timer interrupt settings
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

    ui32Period = SysCtlClockGet();
    ui32Period1 = (SysCtlClockGet() / 10) / 2;

    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
    TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period1 - 1);




    // Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
    LOCK_F=0x4C4F434BU;
    CR_F=GPIO_PIN_0|GPIO_PIN_4;

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);


    /* Configure Buzzer pin as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
    GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_4,GPIO_DIR_MODE_OUT);

    /* Generate a high output on buzzer to turn it off */
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4,0x10);

    /* Configure joystick button */
    GPIOPinTypeGPIOInput(GPIO_PORTE_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTE_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);



    /* Configure the pins connected to LED as output */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2);

    /* Unlock PD7 */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_7);
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK)= GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= 0x80;
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK)= 7;

     /* Configure the pins connected to push buttons as inout */
    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE,GPIO_PIN_6|GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTD_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_7);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);


    /* Turn OFF all the LEDs */
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,1);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5,64);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2,4);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3,8);

}
