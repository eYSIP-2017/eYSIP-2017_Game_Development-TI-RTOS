/*
 * console.h
 *
 *  Created on: 30-May-2017
 *      Author: Akshay U Hegde
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

extern void setup(void);
extern void _init_ADC(void);
extern void _init_GPIO(void);
extern void _init_Timer(void);
extern void _init_Interrupt(void);
extern void _init_(void);
extern void ADC0IntHandler(void);
extern void ADC1IntHandler(void);
extern void millis(uint32_t delay);
extern void micros(uint32_t delay);

#endif /* CONSOLE_H_ */
