/*
 * gameDisplay.h
 *
 *  Created on: 24-Jun-2017
 *      Author: AxRox
 */

#ifndef CONSOLE_GAMEDISPLAY_H_
#define CONSOLE_GAMEDISPLAY_H_

extern void ballDisplay(unsigned char *image);
extern void paddleDisplay(void);
extern void paddleSpecialDisplay(void);
extern void victoryAnimationDisplay(void);
extern void cursorDisplay(void);
extern void blockDisplay(unsigned char blockYPos, unsigned char blockXPos, unsigned char *image);
extern void blockClear(unsigned char blockXPos, unsigned char blockYPos);
extern void blockClearSurround(unsigned char blockX, unsigned char blockY);
extern void screenFlash(unsigned char *image);

#endif /* CONSOLE_GAMEDISPLAY_H_ */
