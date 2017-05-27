/*

 * Author: Texas Instruments

 * Edited by: Saurav Shandilya, Vishwanathan Iyer, Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 3, Problem Statement 1.

 * Filename: lab_3_1.c

 * Functions: setup(), setupPWM(), timerConfig(), interruptConfig(), switchPinConfig(), detectKeyPress(), decreaseBrightness(),
 * increaseBrightness(), colorCycle(), modeDetect(), modeOne(), modeTwo(), modeThree(), main()

 * Global Variables: ui32Load, ui32Period, ui32PWMClock, ui8AdjustR, ui8AdjustG,
 * ui8AdjustB, flag[2], singlePressFlag[2], doublePressFlag[2], delta, delay

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


// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define PWM_FREQUENCY 55
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
 */
volatile uint32_t ui32Load;
volatile uint32_t ui32PWMClock;
volatile uint8_t ui8AdjustR = 240;
volatile uint8_t ui8AdjustG = 10;
volatile uint8_t ui8AdjustB = 10;
unsigned int flag[2] = {0,0};
unsigned int singlePressFlag[2] = {0,0};
unsigned int doublePressFlag[2] = {0,0};
unsigned int delta = 10000;
uint32_t ui32Period;
uint32_t delay = 50000;
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{

    //Set the clock
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    //Configure PWM Clock to match system
    SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

    // Enable the peripherals used by this program.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
}
/*

 * Function Name: setupPWM()

 * Input: none

 * Output: none

 * Description: Configure PWM Generators, set PWM frequency and outputs

 * Example Call: setup();

 */
void setupPWM(void)
{
    //Configure PF1,PF2,PF3 Pins as PWM
    GPIOPinConfigure(GPIO_PF1_M1PWM5);
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);

    // Configure PWM Output
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);


    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

    //Configure PWM Options
    //PWM_GEN_2 Covers M1PWM4 and M1PWM5
    //PWM_GEN_3 Covers M1PWM6 and M1PWM7
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);

    //Set the Period
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

    //Set PWM duty
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);

    // Enable the PWM generator
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

    // Turn on the Output pins
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_6_BIT|PWM_OUT_7_BIT, true);
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

    ui32Period = (SysCtlClockGet() / 4) / 2;
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

 * Description: Set PORTF Pin 0 as input. Note that Pin 0 is locked.

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

enum modes{
    AUTO,
    MODE1,
    MODE2,
    MODE3
};

enum modes mode = AUTO;
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
            if(flag[n] == 1)
            {
                flag[n] = 0;
                state[n] = IDLE;
                if (doublePressFlag[n] == 1) //Double Press Detection
                    doublePressFlag[n] = 0;
                else
                    doublePressFlag[n] = 1;
                return 0;
            }
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
            flag[n] = 1;
            return 0;
        }
    }
}
/*

 * Function Name: decreaseBrightness()

 * Input: uint8_t *ui8Adjust, uint32_t ui32PWMOut

 * Output: None

 * Description: Soft Decrease Brightness of given LED.

 * Example Call: detectKeyPress(ui8AdjustR, GPIO_PIN_5);

 */
void decreaseBrightness(volatile uint8_t *ui8Adjust, uint32_t ui32PWMOut)
{
    while ((*ui8Adjust) > 10)
    {
        (*ui8Adjust)--;
        PWMPulseWidthSet(PWM1_BASE, ui32PWMOut, (*ui8Adjust) * ui32Load / 1000);
        SysCtlDelay(delay);
    }
}
/*

 * Function Name: increaseBrightness()

 * Input: uint8_t *ui8Adjust, uint32_t ui32PWMOut

 * Output: None

 * Description: Soft Increase Brightness of given LED.

 * Example Call: detectKeyPress(ui8AdjustR, GPIO_PIN_5);

 */
void increaseBrightness(volatile uint8_t *ui8Adjust, uint32_t ui32PWMOut)
{
    while ((*ui8Adjust) < 240)
    {
        (*ui8Adjust)++;
        PWMPulseWidthSet(PWM1_BASE, ui32PWMOut, (*ui8Adjust) * ui32Load / 1000);
        SysCtlDelay(delay);
    }
}
/*

 * Function Name: colorCycle()

 * Input: None

 * Output: None

 * Description: Perform sequential transition as per given color cycle.

 * Example Call: colorCycle()

 */
void colorCycle(void)
{
    increaseBrightness(&ui8AdjustG, PWM_OUT_7); //Towards Yellow
    decreaseBrightness(&ui8AdjustR, PWM_OUT_5); //Towards Green
    increaseBrightness(&ui8AdjustB, PWM_OUT_6); //Towards Cyan
    decreaseBrightness(&ui8AdjustG, PWM_OUT_7); //Towards Blue
    increaseBrightness(&ui8AdjustR, PWM_OUT_5); //Towards Magenta
    decreaseBrightness(&ui8AdjustB, PWM_OUT_6); //Towards Red
}
/*

 * Function Name: modeDetect()

 * Input: None

 * Output: None

 * Description: Check the mode of operation based on switch.

 * Example Call: modeDetect()

 */
void modeDetect(void)
{
    if((state[0] == RELEASE) & (state[1] == PRESS)) //MODE1 Detect
    {
        mode = MODE1;
    }
    if((state[0] == RELEASE) & (doublePressFlag[1] == 1)) //MODE2 Detect
    {
        doublePressFlag[1] = 0;
        mode = MODE2;
    }
    if((state[0] == RELEASE) & (state[1] == RELEASE)) //MODE3 Detect
    {
        mode = MODE3;
    }
}
/*

 * Function Name: modeOne()

 * Input: None

 * Output: None

 * Description: Perform basic initialization for Mode 1, set outputs.

 * Example Call: modeOne()

 */
void modeOne(void)
{
    ui8AdjustR = 130; //Set only Red LED Output
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT|PWM_OUT_7_BIT, false);
}
/*

 * Function Name: modeTwo()

 * Input: None

 * Output: None

 * Description: Perform basic initialization for Mode 2, set outputs.

 * Example Call: modeTwo()

 */
void modeTwo(void)
{
    ui8AdjustB = 130; //Set only Blue LED Output
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT|PWM_OUT_7_BIT, false);
}
/*

 * Function Name: modeThree()

 * Input: None

 * Output: None

 * Description: Perform basic initialization for Mode 3, set outputs.

 * Example Call: modeThree()

 */
void modeThree(void)
{
    ui8AdjustG = 130; // Set only Green LED Output
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT|PWM_OUT_5_BIT, false);
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

    modeDetect();
    if(mode == AUTO) //AUTO Mode, Switch Operation - Delay change
    {
        if(detectKeyPress(GPIO_PIN_4, 0))
        {
            if (delay > 20000)
                delay -= delta;
            else
                delay = 20000;
        }
        if(detectKeyPress(GPIO_PIN_0, 1))
        {
            singlePressFlag[1] = 0;
            if (delay < 80000)
                delay += delta;
            else
                delay = 80000;
        }
    }

    if(mode == MODE1) //MODE1 Switch Operation -- Hard toggle Red Brightness
    {
        if(detectKeyPress(GPIO_PIN_4, 0))
        {
            if(ui8AdjustR > 20)
                ui8AdjustR -= 40;
            else
                ui8AdjustR = 20;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
        }
        if(detectKeyPress(GPIO_PIN_0, 1))
        {
            if(ui8AdjustR < 240)
                ui8AdjustR += 40;
            else
                ui8AdjustR = 240;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8AdjustR * ui32Load / 1000);
        }
    }

    if(mode == MODE2) //MODE2 Switch Operation -- Hard toggle Blue Brightness
    {
        if(detectKeyPress(GPIO_PIN_4, 0))
        {
            if(ui8AdjustB > 20)
                ui8AdjustB -= 40;
            else
                ui8AdjustB = 20;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
        }
        if(detectKeyPress(GPIO_PIN_0, 1))
        {
            if(ui8AdjustB < 240)
                ui8AdjustB += 40;
            else
                ui8AdjustB = 240;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8AdjustB * ui32Load / 1000);
        }
    }

    if(mode == MODE3) //MODE3 Switch Operation -- Hard toggle Green Brightness
    {
        if(detectKeyPress(GPIO_PIN_4, 0))
        {
            if(ui8AdjustG > 10)
                ui8AdjustG -= 40;
            else
                ui8AdjustG = 10;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);
        }
        if(detectKeyPress(GPIO_PIN_0, 1))
        {
            if(ui8AdjustG < 240)
                ui8AdjustG += 40;
            else
                ui8AdjustG = 250;
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8AdjustG * ui32Load / 1000);
        }
    }
}
/*

 * Function Name: main()

 * Input: none

 * Output: none

 * Description: Main Function

 * Example Call: main();

 */
int main(void)
{
    //System Configuration Function Call
    setup();
    setupPWM();
    timerConfig();
    interruptConfig();
    switchPinConfig();
    uint8_t flagR,flagB,flagG;

    while(1)
    {
        //Set to different modes of operation
        while(mode == AUTO)
            {
            colorCycle();
            }
        flagR = 0;
        while (mode == MODE1)
            {
            if(flagR != 1)
                modeOne(); //Initial setup. Performed Once.
                flagR = 1;
            }
        flagB = 0;
        while (mode == MODE2)
            {
            if(flagB != 1)
                {
                modeTwo(); //Initial setup. Performed Once.
                flagB = 1;
                }
            }
        flagG = 0;
        while (mode == MODE3)
            {
            if(flagG != 1)
            {
                modeThree(); //Initial setup. Performed Once.
                flagG = 1;
            }
            }
    }
}
