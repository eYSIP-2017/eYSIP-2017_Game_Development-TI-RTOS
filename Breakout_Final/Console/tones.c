/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Despande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: This is the tones library built along the Arduino tones library.
                Can be used to generate beeps on the buzzer at required frequency/notes and also
                to play music. Music can be defined in functions. Certain examples are already included.

 * Filename: tones.c

 * Functions: beep(), blockBeep(), paddleBeep(), entryBeep(), startBeep(), specialEntryBeep(),
              specialExitBeep(), gameOverMusic(), play_GOT(), play_marioBros(), play_maryHadALittleLamb()

 * Global Variables: None

 */

//Standard C Library Header
#include <stdint.h>
#include <stdbool.h>

// Console Library
#include "Console/consoleInit.h"
#include "Console/glcd.h"

// GPIO Functions Library
#include "driverlib/timer.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"

// ROM Library
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

// Frequency definitions over the entire piano scale(Not all are playable using onboard buzzer.)
// Select between 3 to 5 for optimal music.
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
/*

 * Function Name: beep(unsigned int note, unsigned int duration)

 * Input: note, duration

 * Output: none

 * Description: Generate a beep of given frequency/note for a certain duration.

 * Example Call: beep(NOTE_DS4, 200)

 */
void beep(unsigned int note, unsigned int duration)
{
    int i;
    long delay = (long)(76000/note);  //This is the semiperiod of each note.
    long time = (long)((duration*1000)/(delay*2));  //This is how much time we need to spend on the note.
    for (i=0;i<time;i++)
    {
        ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0x00);    //Set PC.4...
        micros(delay);   //...for a semiperiod...
        ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_4, 0x10);    //...then reset it...
        micros(delay);   //...for the other semiperiod.
    }
    millis(20); //Add a little delay to separate the single notes
}
// Beep on hitting a block
void blockBeep()
{
    beep(NOTE_C3, 100);
}
// Beep on hitting a paddle
void paddleBeep()
{
    beep(NOTE_D4, 100);
}
// Music on Entry into Gameplay
void entryBeep()
{
    beep(NOTE_D3, 100);
    beep(NOTE_A3, 300);
}
// Music on Entry into Menu Screen
void startBeep()
{
    beep(NOTE_D3, 100);
    beep(NOTE_F3, 200);
    beep(NOTE_D3, 100);
    beep(NOTE_A3, 400);
}
// Music on Start of Special gameplay upon hitting BLOCK_MAGIC2
void specialEntryBeep()
{
    beep(NOTE_D3, 100);
    beep(NOTE_F3, 100);
    beep(NOTE_C3, 100);
    beep(NOTE_G3, 200);
}
// Music on End of Special gameplay after hitting BLOCK_MAGIC2
void specialExitBeep()
{
    beep(NOTE_G3, 100);
    beep(NOTE_C3, 100);
    beep(NOTE_F3, 100);
    beep(NOTE_D3, 200);
}
// Music on Entry into Game Over Screen
void gameOverMusic()
{
    beep(NOTE_D4, 300);
    beep(NOTE_B3, 400);
    beep(NOTE_A3, 300);
    beep(NOTE_G3, 200);
    beep(NOTE_E3, 300);
    beep(NOTE_D3, 400);
    beep(NOTE_C3, 300);
    beep(NOTE_AS2, 500);
}
// Game of Thrones Theme(ported over from Sheet Music) to be played on VICTORY screen
void play_GOT()
{
    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_DS3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_DS3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_DS3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 300);
    beep(NOTE_C3, 300);
    beep(NOTE_DS3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_G3, 900);

    beep(NOTE_C3, 900);

    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);
    beep(NOTE_G3, 600);

    beep(NOTE_C3, 600);
    beep(NOTE_E3, 150);
    beep(NOTE_F3, 150);

    beep(NOTE_D3, 300);
    beep(NOTE_G2, 300);
    beep(NOTE_B2, 150);
    beep(NOTE_C3, 150);

    beep(NOTE_D3, 300);
    beep(NOTE_G2, 300);
    beep(NOTE_B2, 150);
    beep(NOTE_C3, 150);

    beep(NOTE_D3, 300);
    beep(NOTE_G2, 300);
    beep(NOTE_B2, 150);
    beep(NOTE_C3, 150);

    beep(NOTE_D3, 300);
    beep(NOTE_G2, 300);
    beep(NOTE_B2, 300);

    beep(NOTE_F3, 900);

    beep(NOTE_B2, 900);

    beep(NOTE_E3, 150);
    beep(NOTE_D3, 150);
    beep(NOTE_F3, 600);

    beep(NOTE_B2, 600);
    beep(NOTE_E3, 150);
    beep(NOTE_D3, 150);

    beep(NOTE_C3, 300);
    beep(NOTE_F2, 300);
    beep(NOTE_A2, 150);
    beep(NOTE_B2, 150);

    beep(NOTE_C3, 300);
    beep(NOTE_F2, 300);
    beep(NOTE_A2, 150);
    beep(NOTE_B2, 150);

    beep(NOTE_C3, 300);
    beep(NOTE_F2, 300);
    beep(NOTE_A2, 150);
    beep(NOTE_B2, 150);

    beep(NOTE_C3, 300);
    beep(NOTE_F2, 300);
    beep(NOTE_A2, 300);
}
// Super Mario Bros Theme(ported over from Sheet Music) to be played on initial cutscene
void play_MarioBros()
{
    beep(NOTE_E4, 100);
    beep(NOTE_E4, 100);
    millis(100);
    beep(NOTE_E4, 100);

    millis(100);
    beep(NOTE_C4, 100);
    beep(NOTE_E4, 100);
    millis(100);

    beep(NOTE_G4, 100);
    millis(300);

    beep(NOTE_G3, 100);
    millis(300);

    beep(NOTE_E3, 100);
    millis(200);
    beep(NOTE_C3, 100);

    millis(200);
    beep(NOTE_G2, 100);
    millis(100);

    millis(100);
    beep(NOTE_C3, 100);
    millis(100);
    beep(NOTE_D3, 100);

    millis(100);
    beep(NOTE_D3, 100);
    beep(NOTE_C3, 100);
    millis(100);

    beep(NOTE_G3, 133);
    beep(NOTE_E4, 133);
    beep(NOTE_G4, 133);

    beep(NOTE_A5, 100);
    millis(100);
    beep(NOTE_F4, 100);
    beep(NOTE_G4, 100);

    millis(100);
    beep(NOTE_E4, 100);
    millis(100);
    beep(NOTE_C4, 100);

    beep(NOTE_D4, 100);
    beep(NOTE_B4, 100);
    millis(200);

    beep(NOTE_C4, 100);
    millis(200);
    beep(NOTE_G3, 100);

    millis(200);
    beep(NOTE_E3, 100);
    millis(100);

    millis(100);
    beep(NOTE_A3, 100);
    millis(100);
    beep(NOTE_B3, 100);

    millis(100);
    beep(NOTE_AS3, 100);
    beep(NOTE_A3, 100);
    millis(100);

    beep(NOTE_G3, 133);
    beep(NOTE_E4, 133);
    beep(NOTE_G4, 133);

    beep(NOTE_A5, 100);
    millis(100);
    beep(NOTE_F4, 100);
    beep(NOTE_G4, 100);

    millis(100);
    beep(NOTE_E4, 100);
    millis(100);
    beep(NOTE_C4, 100);

    beep(NOTE_D4, 100);
    beep(NOTE_B4, 100);
    millis(200);
}
// Demo example which plays Mary Had a Little Lamb (out of tempo)
void play_maryHadALittleLamb()
{
    beep(NOTE_E4, 500);
    beep(NOTE_D4, 400);
    beep(NOTE_C4, 400);
    beep(NOTE_D4, 400);

    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);

    millis(150);

    beep(NOTE_D4, 400);
    beep(NOTE_D4, 400);
    beep(NOTE_D4, 400);

    millis(150);

    beep(NOTE_E4, 400);
    beep(NOTE_G4, 400);
    beep(NOTE_G4, 400);

    beep(NOTE_E4, 500);
    beep(NOTE_D4, 400);
    beep(NOTE_C4, 400);
    beep(NOTE_D4, 400);

    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_D4, 400);
    beep(NOTE_D4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_E4, 400);
    beep(NOTE_C4, 600);
}
