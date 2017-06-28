/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: The Header file for access to different object display functions

 * Filename: gameDisplay.h

 */

#ifndef CONSOLE_GAMEDISPLAY_H_
#define CONSOLE_GAMEDISPLAY_H_

extern void ballDisplay(unsigned char *image);
extern void paddleDisplay(void);
extern void paddleSpecialDisplay(void);
extern void cursorDisplay(void);
extern void blockDisplay(unsigned char blockYPos, unsigned char blockXPos, unsigned char *image);
extern void blockClear(unsigned char blockXPos, unsigned char blockYPos);
extern void blockClearSurround(unsigned char blockX, unsigned char blockY);

#endif /* CONSOLE_GAMEDISPLAY_H_ */
