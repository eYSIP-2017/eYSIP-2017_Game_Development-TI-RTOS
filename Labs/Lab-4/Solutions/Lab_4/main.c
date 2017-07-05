/*

 * Author: Texas Instruments

 * Edited by: Akshay U Hegde
          ERTS Lab, CSE Department, IIT Bombay

 * Description: This is the solution for Lab 4

 * Filename: lab_4.c

 * Functions: setup(), _init_ADC(), _init_UART(), _init_Interrupt(), sendUART(), ADC0IntHandler(), ADC1IntHandler(), main()

 * Global Variables: ui32ADC0Value[4], ui32ADC1Value[4], ui8YAxisAvg, ui8XAxisAvg, flag

 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

//For Definition of ROM functions
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

/*
 * Global Variable Definition
 */
uint32_t ui32ADC0Value[4];
uint32_t ui32ADC1Value[4];
uint8_t ui8YAxisAvg;
uint8_t ui8XAxisAvg;
uint8_t flag;
/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup()
{
    //Set Clock to run at 40MHz.
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

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

    ROM_GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
}
/*

 * Function Name: _init_UART()

 * Input: none

 * Output: none

 * Description: Configure UART pins, initialize UART Clock and Baud.

 * Example Call: _init_UART();

 */
void _init_UART(void)
{
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}
/*

 * Function Name: _init_Interrupt()

 * Input: none

 * Output: none

 * Description: Enable ADC Interrupts.

 * Example Call: _init_Interrupt();

 */
void _init_Interrupt(void)
{
    ROM_IntMasterEnable();

    ROM_IntEnable(INT_ADC0SS1);
    ROM_IntEnable(INT_ADC1SS1);

    ROM_ADCIntEnable(ADC0_BASE, 1);
    ROM_ADCIntEnable(ADC1_BASE, 1);
}
/*

 * Function Name: sendUART()

 * Input: none

 * Output: none

 * Description: Package UART Data and send in the form of 3-byte frame packets with header.

 * Example Call: sendUART();

 */
void sendUART()
{
    uint8_t checksum, i;
    char frame[3] = {0x7E, ui8XAxisAvg, ui8YAxisAvg};
    for(i = 0; i < 3; i++)
    {
        UARTCharPut(UART0_BASE, frame[i]);
    }
}
/*

 * Function Name: ADC0IntHandler()

 * Input: none

 * Output: none

 * Description: ADC 0 Interrupt Service Routine. Averages Y-Axis ADC Input at 4 channels.

 * Example Call: ADC0IntHandler();

 */
void ADC0IntHandler(void)
{
    ROM_ADCIntClear(ADC0_BASE, 1);
    ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);

    ui8YAxisAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/512;
    flag = 1;
}
/*

 * Function Name: ADC1IntHandler()

 * Input: none

 * Output: none

 * Description: ADC 1 Interrupt Service Routine. Averages X-Axis ADC Input at 4 channels.

 * Example Call: ADC1IntHandler();

 */
void ADC1IntHandler(void)
{
    ROM_ADCIntClear(ADC1_BASE, 1);
    ROM_ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);

    ui8XAxisAvg = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3] + 2)/32;
    if(flag == 1)
    {
        flag = 0;
        sendUART();
    }
}

void main()
{
    setup();
    _init_ADC();
    _init_UART();
    _init_Interrupt();

    while(1)
    {   //Use Processor Trigger for ADCs
        ROM_ADCProcessorTrigger(ADC0_BASE, 1);
        ROM_ADCProcessorTrigger(ADC1_BASE, 1);
    }
}
