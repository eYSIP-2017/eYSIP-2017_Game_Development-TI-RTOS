/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the header file for the console which initializes and sets required peripherals.

 * Filename: consoleInit.h

 */
#ifndef CONSOLE_H_
#define CONSOLE_H_

extern void setup(void);
extern void _init_ADC(void);
extern void _init_GPIO(void);
extern void _init_Timer(void);
extern void _init_Interrupt(void);
extern void millis(unsigned int delay);
extern void micros(unsigned int delay);
extern void switchSelect(unsigned char index);
extern unsigned int detectKeyPress(unsigned char n);
extern void ledOFF(unsigned char index);
extern void ledON(unsigned char index);
extern void motorOFF(void);
extern void motorON(void);
extern void _init_(void);

#endif /* CONSOLE_H_ */
