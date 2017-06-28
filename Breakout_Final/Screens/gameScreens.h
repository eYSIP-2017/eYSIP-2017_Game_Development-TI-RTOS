/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: Various Game screens to be displayed are defined as a header, including functions to display them.

 * Filename: gameScreens.h

 */

#ifndef SCREENS_GAMESCREENS_H_
#define SCREENS_GAMESCREENS_H_

extern unsigned char gameOverScreen[1024];
extern unsigned char menuScreen[1024];
extern unsigned char titleScreen[1024];
extern unsigned char settingsScreen[1024];
extern unsigned char creditsScreen[1024];
extern unsigned char threeLivesScreen[1024];
extern unsigned char twoLivesScreen[1024];
extern unsigned char oneLifeScreen[1024];
extern unsigned char victoryScreen1[1024];
extern unsigned char victoryScreen2[1024];
extern unsigned char victoryScreen3[1024];
extern unsigned char victoryScreen4[1024];
extern void gameScreen(void);
extern void victoryAnimationDisplay(void);
extern void screenFlash(unsigned char *image);
extern void gameScreenRefresh(void);

#endif /* SCREENS_GAMESCREENS_H_ */
