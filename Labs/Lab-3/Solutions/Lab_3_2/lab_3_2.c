/*

 * Author: Texas Instruments

 * Edited by: Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the tentative solution for Lab 3, Problem Statement 2.

 * Filename: lab_3_2.c

 * Functions: setup(), setupPWM(), timerConfig(), interruptConfig(), switchPinConfig(), detectKeyPress(), Timer0IntHandler(), main()

 * Global Variables: ui32Load, ui32Period, ui32PWMClock, ui8Adjust

 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"

// PWM_FREQUENCY to set PWM frequency
#define PWM_FREQUENCY 50
/*
 ------ Global Variable Declaration
 */
volatile uint32_t ui32Load;
volatile uint32_t ui32PWMClock;
volatile uint32_t ui8Adjust = 750;
volatile uint32_t ui32Period;
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
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
}
/*

 * Function Name: setupPWM()

 * Input: none

 * Output: none

 * Description: Configure PWM Generators, set PWM frequency and outputs

 * Example Call: setupPWM();

 */
void setupPWM()
{

    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PD0_M1PWM0);

    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, ui32Load);

    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 10000);
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);
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
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*
 * State values and state initialization for State Machine
 */
enum states{

    IDLE,
    PRESS,
    RELEASE
};

enum states state[2] = {IDLE, IDLE};
/*

 * Function Name: detectKeyPress()

 * Input: uint32_t pinName, unsigned int N

 * Output: unsigned int flag

 * Description: State Machine which performs Switch Debouncing, for accurate switch press detection.

 * Example Call: detectKeyPress(GPIO_PIN_4, 0);

 */
unsigned int detectKeyPress(uint32_t pinName, unsigned int n)
{
    switch(state[n]){
    case IDLE:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state[n] = PRESS;
            return 0;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case PRESS:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 1;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case RELEASE:
        if(GPIOPinRead(GPIO_PORTF_BASE,pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 1;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    }
}

void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    if(detectKeyPress(GPIO_PIN_4, 0)) // 10 degrees in clockwise direction
    {
        ui8Adjust -= 28;
        if (ui8Adjust < 500)
        {
            ui8Adjust = 500;
        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 10000);
    }
    if(detectKeyPress(GPIO_PIN_0, 1)) // 10 degrees in anti clockwise direction
    {
        ui8Adjust += 28;
        if (ui8Adjust > 1000)
        {
            ui8Adjust = 1000;
        }
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * ui32Load / 10000);
    }
    SysCtlDelay(100000);
}


void main(void) {

    setupPWM();
    switchPinConfig();

    while(1)
    {
    }
}
