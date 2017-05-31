/*

 * Author: Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the initialization definitions of the header file for the console which initializes and sets required peripherals.

 * Filename: console.c

 * Functions: setup(), _init_ADC(), _init_GPIO(), _init_Timer, _init_Interrupt(), _init_(), ADC0IntHandler(), ADC1IntHandler(),
 *            delay_ms(), delay_ns()

 * Global Variables: ui32Period, mdelay, ndelay, ui32ADC0Value[4], ui32ADC1Value[4], ui8YAxisAvg, ui8XAxisAvg

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
uint32_t ui32Period;
uint32_t mdelay;
uint32_t ndelay;
uint32_t ui32ADC0Value[4];
uint32_t ui32ADC1Value[4];
uint8_t ui8YAxisAvg;
uint8_t ui8XAxisAvg;
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup()
{

    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
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
    //Average a larger number of samples
    ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);
    ROM_ADCHardwareOversampleConfigure(ADC1_BASE, 64);

    //Sequencer Configuration for each module
    ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
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
    ROM_ADCSequenceEnable(ADC0_BASE, 1);
    ROM_ADCSequenceEnable(ADC1_BASE, 1);

    // Initial Interrupt Clear
    ROM_ADCIntClear(ADC0_BASE, 1);
    ROM_ADCIntClear(ADC1_BASE, 1);
}
/*

 * Function Name: _init_GPIO()

 * Input: none

 * Output: none

 * Description: Configure and set GPIO pins for the console.

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

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_6);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5);
    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3);

    ROM_GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_7,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_6|GPIO_PIN_7,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_DIR_MODE_IN);
    ROM_GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_5|GPIO_PIN_6, GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_3,GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    ROM_GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3,GPIO_DIR_MODE_OUT);

    ROM_GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_7,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_6|GPIO_PIN_7,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
    ROM_GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

    ROM_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_UARTConfigSetExpClk(UART0_BASE, ROM_SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
}
/*

 * Function Name: _init_Timer()

 * Input: none

 * Output: none

 * Description: Initialize Timer 0.

 * Example Call: _init_Timer();

 */
void _init_Timer(void)
{

    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    ui32Period = (ROM_SysCtlClockGet() / 20) / 2;
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);

}
/*

 * Function Name: _init_Interrupt()

 * Input: none

 * Output: none

 * Description: Enable ADC Interrupts for Thumb Joystick.

 * Example Call: _init_Interrupt();

 */
void _init_Interrupt(void)
{

    ROM_IntMasterEnable();

    ROM_IntEnable(INT_TIMER0A);
    ROM_IntEnable(INT_UART0);
    ROM_IntEnable(INT_ADC0SS1);
    ROM_IntEnable(INT_ADC1SS1);

    ROM_ADCIntEnable(ADC0_BASE, 1);
    ROM_ADCIntEnable(ADC1_BASE, 1);
    ROM_UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);

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
    _init_Interrupt();
}

/*

 * Function Name: ADC0IntHandler()

 * Input: none

 * Output: none

 * Description: ADC 0 Interrupt Service Routine. Averages Y-Axis ADC Input at 4 channels and scales it to 0-7

 * Example Call: ADC0IntHandler();

 */
void ADC0IntHandler(void)
{
    ROM_ADCIntClear(ADC0_BASE, 1);
    ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

    ui8YAxisAvg = 8 - ((ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3])/2048);
}
/*

 * Function Name: ADC1IntHandler()

 * Input: none

 * Output: none

 * Description: ADC 1 Interrupt Service Routine. Averages X-Axis ADC Input at 4 channels and scales it to 0 - 128.

 * Example Call: ADC1IntHandler();

 */
void ADC1IntHandler(void)
{
    ROM_ADCIntClear(ADC1_BASE, 1);
    ROM_ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);

    ui8XAxisAvg = 128 - ((ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3])/128);
}
/*

 * Function Name: delay_ms()

 * Input: none

 * Output: none

 * Description: Adds delay in multiples of 1 ms.

 * Example Call: delay_ms();

 */
void delay_ms(uint32_t delay)
{
    mdelay = delay*1340000;
    ROM_SysCtlDelay(mdelay);
}
/*

 * Function Name: delay_ns()

 * Input: none

 * Output: none

 * Description: Adds delay in multiples of 1 ns.

 * Example Call: delay_ns();

 */
void delay_ns(uint32_t delay)
{
    ndelay = delay*134;
    ROM_SysCtlDelay(ndelay);
}
