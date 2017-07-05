/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the header file for the console initialization functions.

 * Filename: console.h

 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

extern void setup(void);
extern void _init_ADC(void);
extern void _init_GPIO(void);
extern void _init_Timer(void);
extern void _init_Interrupt(void);
extern void _init_(void);
extern void millis(uint32_t delay);
extern void micros(uint32_t delay);
extern void switchSelect(unsigned char index);
extern unsigned int detectKeyPress(unsigned char n);
extern void ledOFF(unsigned char index);
extern void ledON(unsigned char index);

#endif /* CONSOLE_H_ */
