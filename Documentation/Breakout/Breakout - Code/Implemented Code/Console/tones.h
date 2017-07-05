/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Despande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the tones library built along the lines of the Arduino tones library.
                Can be used to generate beeps on the buzzer at required frequency/notes and also
                to play music.

 * Filename: tones.h

 */

#ifndef TONES_H_
#define TONES_H_

extern void beep(unsigned int note, unsigned int duration);
extern void play_maryHadALittleLamb();
extern void play_GOT();
extern void play_MarioBros();
extern void blockBeep();
extern void paddleBeep();
extern void specialEntryBeep();
extern void specialExitBeep();
extern void gameOverMusic();
extern void startBeep();
extern void entryBeep();

#endif /* TONES_H_ */
