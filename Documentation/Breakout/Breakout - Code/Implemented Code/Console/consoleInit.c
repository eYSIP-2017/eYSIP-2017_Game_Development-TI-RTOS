/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the initialization definitions of the header file for the console which initializes and sets required peripherals.

 * Filename: consoleInit.c

 * Functions: setup(), _init_ADC(), _init_GPIO(), _init_Timer, _init_Interrupt(), millis(), micros(),
              switchSelect(), detectKeyPress(), ledOFF(), ledON(), motorOFF(), motorON(), _init_()

 * Global Variables: ui32Period1, ui32Period2, ui32Period5, pinName, baseName, mdelay, ndelay, ui8YAxisAvg, ui8XAxisAvg

 */
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

/*
 ------ Global Variable Declaration
 */
uint32_t ui32Period1, ui32Period2, ui32Period5, pinName, baseName, mdelay, ndelay;
uint8_t ui8YAxisAvg, ui8XAxisAvg;
/*
 * Enumeration of States for State Machines
 */
enum states{

    IDLE,
    PRESS,
    RELEASE
};
// Initialization
enum states state[5] = {IDLE, IDLE, IDLE, IDLE, IDLE};
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO, ADC, Timer Peripherals.

 * Example Call: setup();

 */
void setup()
{

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    // ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
}

/*

 * Function Name: _init_ADC()

 * Input: none

 * Output: none

 * Description: Configure ADC Samplers, set ADC Interrupts, Inputs and Enable ADC.

 * Example Call: _init_ADC();

 */
void _init_ADC()
{
    /*    //Average a larger number of samples
    // ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    ROM_ADCHardwareOversampleConfigure(ADC1_BASE, 64);

    // Sequencer Configuration for each module
    // ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 1);

    // Take 4 inputs and average over them for each input
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END);

    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 3, ADC_CTL_CH6 | ADC_CTL_IE | ADC_CTL_END);

    //Enable ADC Sequencer
    // ROM_ADCSequenceEnable(ADC0_BASE, 1);
    //ROM_ADCSequenceEnable(ADC1_BASE, 1);

    // Initial Interrupt Clear
    // ROM_ADCIntClear(ADC0_BASE, 1);
    //ROM_ADCIntClear(ADC1_BASE, 1);
    Enable the ADC */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);

    /* Configure and enable the ADC */
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE,3,0,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCSequenceConfigure(ADC1_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC1_BASE,3,0,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC1_BASE, 3);

}
/*

 * Function Name: _init_GPIO()

 * Input: none

 * Output: none

 * Description: Configure and set all GPIO pins for the console.

 * Example Call: _init_GPIO();

 */
void _init_GPIO()
{

    HWREG(GPIO_PORTC_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTC_BASE+GPIO_O_CR) |= (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    HWREG(GPIO_PORTD_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE+GPIO_O_CR) |= GPIO_PIN_7;

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

    ROM_GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_7);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6|GPIO_PIN_7);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_4);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3);

    ROM_GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_7,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_6|GPIO_PIN_7,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_3,GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3,GPIO_DIR_MODE_OUT);

    ROM_GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6|GPIO_PIN_7,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

    ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0x10);

    ROM_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);

    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}
/*

 * Function Name: _init_Timer()

 * Input: none

 * Output: none

 * Description: Initialize Timer 1, Timer 2 and Timer 5.

 * Example Call: _init_Timer();

 */
void _init_Timer(void)
{

    ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerConfigure(TIMER5_BASE, TIMER_CFG_PERIODIC);

    ui32Period1 = (ROM_SysCtlClockGet()/1000);
    ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period1 -1);

    ui32Period2 = (ROM_SysCtlClockGet()/1000);
    ROM_TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period2-1);           // set Timer 2 period

    ui32Period5 = (ROM_SysCtlClockGet()/1);
    ROM_TimerLoadSet(TIMER5_BASE,TIMER_A, ui32Period5-1);

    // ROM_TimerEnable(TIMER1_BASE, TIMER_A); // Initialized explicitly in SPECIAL gameplayChange
    ROM_TimerEnable(TIMER2_BASE, TIMER_A);
}
/*

 * Function Name: _init_Interrupt()

 * Input: none

 * Output: none

 * Description: Enable ADC and Timer Interrupts.

 * Example Call: _init_Interrupt();

 */
void _init_Interrupt(void)
{

    ROM_IntMasterEnable();

    // ROM_IntEnable(INT_TIMER1A);
    ROM_IntEnable(INT_TIMER2A);
    ROM_IntEnable(INT_TIMER5A);
    // ROM_IntEnable(INT_ADC0SS1);
    ROM_IntEnable(INT_ADC1SS1);
    ROM_IntEnable(INT_UART0);
    // ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    ROM_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);        // enables Timer 2 to interrupt CPU
    // ROM_TimerIntEnable(TIMER5_BASE, TIMER_TIMA_TIMEOUT);
    ROM_ADCIntEnable(ADC1_BASE, 1);

    // ROM_ADCIntEnable(ADC0_BASE, 1); // Not Required
    // ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

}
/*

 * Function Name: millis()

 * Input: none

 * Output: none

 * Description: Adds delay in multiples of 1 ms.

 * Example Call: millis();

 */
void millis(unsigned int delay)
{
    mdelay = delay*(ROM_SysCtlClockGet()/3000);
    ROM_SysCtlDelay(mdelay);
}
/*
 * Function Name: micros()

 * Input: none

 * Output: none

 * Description: Adds delay in multiples of 1 us.

 * Example Call: micros();

 */
void micros(unsigned int delay)
{
    ndelay = delay*(ROM_SysCtlClockGet()/2678000);
    ROM_SysCtlDelay(ndelay);
}
/*

 * Function Name: switchSelect()

 * Input: unsigned char index

 * Output: None

 * Description: Change baseName, pinName variables to desired switch based on selected switch number.

 * Example Call: switchSelect(0);

 */
void switchSelect(unsigned char index)
{
    switch(index){
    case 0:
        baseName = GPIO_PORTC_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 1:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 2:
        baseName = GPIO_PORTF_BASE;
        pinName = GPIO_PIN_4;
        break;
    case 3:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_6;
        break;
    case 4:
        baseName = GPIO_PORTE_BASE;
        pinName = GPIO_PIN_4;
        break;
    }
}
/*

 * Function Name: detectKeyPress()

 * Input: unsigned char n

 * Output: unsigned int flag

 * Description: State Machine which performs Switch de bouncing, for accurate switch press detection.

 * Example Call: detectKeyPress(0);

 */
unsigned int detectKeyPress(unsigned char n)
{
    // Set baseName and pinName
    switchSelect(n);
    switch(state[n]){
    case IDLE:
        // Switch not pressed
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            //If Switch Press detected, change state to PRESS
            state[n] = PRESS;
            return 0;
        }
        else
        {
            // No switch press
            state[n] = IDLE;
            return 0;
        }
    case PRESS:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            // If Switch is still pressed, change state to RELEASE, switch detected as pressed.
            state[n] = RELEASE;
            return 1;
        }
        else
        {
            // Switch bounce detected, so return to IDLE
            state[n] = IDLE;
            return 0;
        }
    case RELEASE:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            // Switch continues to be pressed
            state[n] = RELEASE;
            return 0;
        }
        else
        {
            // Switch released, so go back to idle
            state[n] = IDLE;
            return 0;
        }
    }
}
/*

 * Function Name: ledOFF()

 * Input: unsigned char index

 * Output: None

 * Description: Turns off requisite LED, as per given LED number(from left to right on the console).

 * Example Call: ledOFF(1);

 */
void ledOFF(unsigned char index)
{
    // Switch Case for different LEDs
    switch(index){
    case 1:
        ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x02);
        break;
    case 2:
        ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x20);
        break;
    case 3:
        ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0x04);
        break;
    case 4:
        ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x08);
        break;
    }
}
/*

 * Function Name: ledON()

 * Input: unsigned char index

 * Output: None

 * Description: Turns on requisite LED, as per given LED number(from left to right on the console).

 * Example Call: ledON(1);

 */
void ledON(unsigned char index)
{
    // Switch Case for different LEDs
    switch(index){
    case 1:
        ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x00);
        break;
    case 2:
        ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x00);
        break;
    case 3:
        ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0x00);
        break;
    case 4:
        ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x00);
        break;
    }
}
/*

 * Function Name: motorOFF()

 * Input: None

 * Output: None

 * Description: Turns off the vibration motor.

 * Example Call: motorOFF();

 */
void motorOFF()
{
    ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3, 0x08);
}
/*

 * Function Name: motorON()

 * Input: None

 * Output: None

 * Description: Turns on the vibration motor.

 * Example Call: motorON();

 */
void motorON()
{
    ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3, 0x00);
}
/*

 * Function Name: _init_()

 * Input: none

 * Output: none

 * Description: Initialize clock and set peripherals for the console.

 * Example Call: _init_();

 */
void _init_()
{
    setup();
    _init_ADC();
    _init_GPIO();
    _init_Timer();
    _init_Interrupt();

    motorOFF();
    ledOFF(1);
    ledOFF(2);
    ledOFF(3);
    ledOFF(4);
}

