
/* XDC module Headers */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

#include <xdc/runtime/Log.h>                //needed for any Log_info() call
#include <xdc/cfg/global.h>                 //header file for statically defined objects/handles
#include <xdc/runtime/Log.h>                //needed for any Log_info() call


/* BIOS module Headers */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* Include header files for adc and GPIO functions */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/debug.h"
#include "driverlib/pin_map.h"
//#include "inc/hw_gpio.h"
#include "driverlib/adc.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"



#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include <time.h>

#include <inc/hw_gpio.h>

#include "driverlib/uart.h"

#include "Console/consoleInit.h"
#include "Console/glcd.h"
#include "Console/gameDisplay.h"
#include "Console/tones.h"
#include "Objects/gameObjects.h"
#include "Screens/gameScreens.h"

/* Global Variables
 */

uint32_t ui32ADC0Value;
uint32_t latency, tickCount, delayCount,specialTimeCount, userTickCount;
uint8_t ui8XAxisAvg;
uint8_t ui8XAxisPrev = 64;
unsigned char x, y, index, flagP, flagI;
signed char hit[64];
signed int paddleXPos, paddleXPrev;
unsigned char victoryCheck, ballXPos,ballYPos, ballXPrev, ballYPrev, flagBall = 0;
float ballXMapFloat;
uint32_t pinName, baseName;
unsigned int ballXMapInt, scoreInt, lifeCount = 3,screenRefreshCount = 0, ballMovSpeed = 325, paddleSpeed = 2;
char score_str[4];
volatile uint32_t milliCount, microCount;

enum modes
{
    MENU,
    INSTRUCTIONS,
    SETTINGS,
    GAMEPLAY,
    VICTORY,
    GAMEOVER
};
enum modes mode = MENU;

enum menuModes
{
    ENTRY,
    THROUGHOUT
};

enum menuModes menuMode = ENTRY;

enum gameplayChanges
{
    NORMAL,
    SPECIAL
};

enum gameplayChanges gameplayChange;

enum gameplayModes
{
    ENTER,
    DEATH,
    REENTER,
    ALWAYS
};

enum gameplayModes gameplayMode = ENTER;

enum cursorPositions
{
    ONE,
    TWO,
    THREE
};

enum cursorPositions cursorPos;

enum difficulties
{
    EASY,
    MEDIUM,
    HARD
};
enum difficulties difficulty;

enum ballSpeeds
{
    SLOW,
    FAST,
    FASTER
};

enum ballSpeeds ballSpeed = FAST;

enum directions
{
    INIT,
    LEFT,
    UP_LEFT,
    UP,
    UP_RIGHT,
    RIGHT,
    DOWN_RIGHT,
    DOWN,
    DOWN_LEFT,
};

enum directions direction = INIT;

enum blocks
{
    NONE,
    BLOCK_HARD,
    BLOCK_MED,
    BLOCK_EASY,
    BLOCK_MAGIC1,
    BLOCK_MAGIC2
};

enum blocks block[80];

enum states{

    IDLE,
    PRESS,
    RELEASE
};

enum states state[5] = {IDLE, IDLE, IDLE, IDLE, IDLE};

void switchSelect(unsigned char index)
{
    switch(index){
    case 0:
        baseName = GPIO_PORTC_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 1:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_7;
        break;
    case 2:
        baseName = GPIO_PORTF_BASE;
        pinName = GPIO_PIN_4;
        break;
    case 3:
        baseName = GPIO_PORTD_BASE;
        pinName = GPIO_PIN_6;
        break;
    case 4:
        baseName = GPIO_PORTE_BASE;
        pinName = GPIO_PIN_4;
        break;
    }
}

unsigned int detectKeyPress(unsigned char n)
{
    switchSelect(n);
    switch(state[n]){
    case IDLE:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = PRESS;
            return 0;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case PRESS:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 1;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    case RELEASE:
        if(GPIOPinRead(baseName, pinName) == 0x00)
        {
            state[n] = RELEASE;
            return 0;
        }
        else
        {
            state[n] = IDLE;
            return 0;
        }
    }
}
void ballMap(void)
{
    ballXMapFloat = (ballXPos/1.6);
    ballXMapInt = (int)ballXMapFloat;
    if((ballXMapFloat - ballXMapInt) >= 0.5)
        ballXMapInt--;
}
void ballMapv2(void)
{
    ballXMapFloat = (ballXPos/2);
    ballXMapInt = (int)ballXMapFloat;
    if((ballXMapFloat - ballXMapInt) >= 0.5)
        ballXMapInt--;
}
void ballMovement(void)
{
    volatile unsigned char arrayCount = 0;
    victoryCheck = 1;
    for(arrayCount = 0; arrayCount < 32; arrayCount++)
    {
        if(block[arrayCount] != NONE)
        {
            victoryCheck = 0;
            break;
        }
    }
    switch(direction){
    case INIT:
        ballDisplay(ballInit);
        direction = UP_LEFT;
        break;
    case UP_LEFT:
        if(ballXPos > 0)
            ballXPos--;
        if (ballYPos > 0)
            ballYPos--;
        ballMapv2();
        if(block[ballXMapInt + (8*(ballYPos+1))] != NONE)
        {
            blockClear(ballXMapInt, (ballYPos+1));
            direction = UP_RIGHT;
            ballXPos++;
            ballYPos++;
            break;
        }
        else if(block[ballXMapInt + (8*ballYPos)] != NONE)
        {
            blockClear(ballXMapInt, ballYPos);
            direction = DOWN_LEFT;
            ballXPos++;
            ballYPos++;
            break;
        }
        else
        {
            ballDisplay(ball3);
            millis(ballMovSpeed/2);
            ballDisplay(ball1);
            millis(ballMovSpeed/2);
            if(ballXPos == 0)
            {
                direction = UP_RIGHT;
                if(ballYPos == 0)
                {
                    direction = DOWN_RIGHT;
                    break;
                }
                break;
            }
            if(ballYPos == 0)
                direction = DOWN_LEFT;
            break;
        }
    case UP_RIGHT:
        if(ballXPos < 15)
            ballXPos++;
        if (ballYPos > 0)
            ballYPos--;
        ballMapv2();
        if(block[ballXMapInt + (8*(ballYPos+1))] != NONE)
        {
            blockClear(ballXMapInt, (ballYPos+1));
            direction = UP_LEFT;
            ballXPos--;
            ballYPos++;
            break;
        }
        else if(block[ballXMapInt + (8*ballYPos)] != NONE)
        {
            blockClear(ballXMapInt, ballYPos);
            direction = DOWN_RIGHT;
            ballXPos--;
            ballYPos++;
            break;
        }
        else
        {
            ballDisplay(ball3);
            millis(ballMovSpeed/2);
            ballDisplay(ball0);
            millis(ballMovSpeed/2);
            if(ballXPos == 15)
            {
                direction = UP_LEFT;
                if(ballYPos == 0)
                {
                    direction = DOWN_LEFT;
                    break;
                }
            }
            if(ballYPos == 0)
                direction = DOWN_RIGHT;
            break;
        }
    case DOWN_RIGHT:
        if(ballXPos < 15)
            ballXPos++;
        if (ballYPos < 7)
            ballYPos++;
        ballMapv2();
        if(block[ballXMapInt + (8*(ballYPos-1))] != NONE)
        {
            blockClear(ballXMapInt, (ballYPos-1));
            direction = DOWN_LEFT;
            ballXPos--;
            ballYPos--;
            break;
        }
        else if(block[ballXMapInt + (8*ballYPos)] != NONE)
        {
            blockClear(ballXMapInt, ballYPos);
            direction = UP_RIGHT;
            ballXPos--;
            ballYPos--;
            break;
        }
        else
        {
            ballDisplay(ball0);
            millis(ballMovSpeed/2);
            if(ballXPos == 15)
            {
                direction = DOWN_LEFT;
            }
            if(ballYPos == 7)
            {
                switch(gameplayChange){
                case NORMAL:
                    if((ballXPos >= ((paddleXPos-6)/8)) && (ballXPos <= ((paddleXPos + 6)/8)))
                    {
                        direction = UP_RIGHT;
                        paddleBeep();
                        break;
                    }
                    else
                    {
                        gameplayMode = DEATH;
                        break;
                    }
                case SPECIAL:
                    if((ballXPos >= ((paddleXPos-12)/8)) && (ballXPos <= ((paddleXPos + 12)/8)))
                    {
                        direction = UP_RIGHT;
                        paddleBeep();
                        break;
                    }
                    else
                    {
                        gameplayMode = DEATH;
                        break;
                    }
                }
            }
            ballDisplay(ball3);
            millis(ballMovSpeed/2);
            break;
        }
    case DOWN_LEFT:
        if(ballXPos > 0)
            ballXPos--;
        if (ballYPos < 7)
            ballYPos++;
        ballMapv2();
        if(block[ballXMapInt + (8*(ballYPos-1))] != NONE)
        {
            blockClear(ballXMapInt, (ballYPos-1));
            direction = DOWN_RIGHT;
            ballXPos++;
            ballYPos--;
            break;
        }
        else if(block[ballXMapInt + (8*ballYPos)] != NONE)
        {
            blockClear(ballXMapInt, ballYPos);
            direction = UP_LEFT;
            ballXPos++;
            ballYPos--;
            break;
        }
        else
        {
            ballDisplay(ball3);
            millis(ballMovSpeed/2);
            if(ballXPos == 0)
            {
                direction = DOWN_RIGHT;
            }
            if(ballYPos == 7)
            {
                switch(gameplayChange){
                case NORMAL:
                    if((ballXPos >= ((paddleXPos-6)/8)) && (ballXPos <= ((paddleXPos + 6)/8)))
                    {
                        direction = UP_LEFT;
                        paddleBeep();
                        break;
                    }
                    else
                    {
                        gameplayMode = DEATH;
                        break;
                    }
                case SPECIAL:
                    if((ballXPos >= ((paddleXPos-12)/8)) && (ballXPos <= ((paddleXPos + 12)/8)))
                    {
                        direction = UP_LEFT;
                        paddleBeep();
                        break;
                    }
                    else
                    {
                        gameplayMode = DEATH;
                        break;
                    }
                }
            }
            ballDisplay(ball2);
            millis(ballMovSpeed/2);
            break;

        }
    }
}

void readInput(void)
{
    while(1)
    {

        /***Semaphore_pend(*sem, wait/timeout) decrements the semaphore by 1.
         *  Until semaphore value is zero task is blocked.
         *
         */
        Semaphore_pend(readInputSem, BIOS_WAIT_FOREVER);

        switch(mode){
        case MENU:
            switch(cursorPos){
            case ONE:
                if(detectKeyPress(1) == 1)
                {
                    mode = GAMEPLAY;
                    gameplayMode = ENTER;
                    entryBeep();
                    glcd_clearDisplay();
                }
                else if(detectKeyPress(2) == 1)
                {
                    cursorPos = TWO;
                    blockBeep();
                }
                break;
            case TWO:
                if(detectKeyPress(1) == 1)
                {
                    flagI = 0;
                    mode = INSTRUCTIONS;
                    entryBeep();
                    glcd_clearDisplay();
                }
                else if(detectKeyPress(0) == 1)
                {
                    cursorPos = ONE;
                    blockBeep();
                }
                else if(detectKeyPress(2) == 1)
                {
                    cursorPos = THREE;
                    blockBeep();
                }
                break;
            case THREE:
                if(detectKeyPress(1) == 1)
                {
                    flagI = 0;
                    mode = SETTINGS;
                    entryBeep();
                    glcd_clearDisplay();
                    glcd_write(settingsScreen);
                    cursorPos = ONE;
                }
                else if(detectKeyPress(0) == 1)
                {
                    cursorPos = TWO;
                    blockBeep();
                }
                break;
            }
            break;
            case INSTRUCTIONS:
                if(detectKeyPress(3) == 1)
                {
                    menuMode = ENTRY;
                    mode = MENU;
                    glcd_clearDisplay();
                }
                break;
            case SETTINGS:
                switch(cursorPos){
                case ONE:
                    if(detectKeyPress(1) == 1)
                        difficulty = HARD;
                    else if(detectKeyPress(3) == 1)
                        difficulty = EASY;
                    else if(detectKeyPress(4) == 1)
                        difficulty = MEDIUM;
                    else if(detectKeyPress(0) == 1)
                    {
                        mode = MENU;
                        menuMode = ENTRY;
                    }
                    else if(detectKeyPress(2) == 1)
                        cursorPos = TWO;
                    break;
                case TWO:
                    if(detectKeyPress(1) == 1)
                    {
                        ballSpeed = FASTER;
                        ballMovSpeed = 175;
                        paddleSpeed = 4;
                    }
                    else if(detectKeyPress(3) == 1)
                    {
                        ballSpeed = SLOW;
                        ballMovSpeed = 325;
                        paddleSpeed = 2;
                    }
                    else if(detectKeyPress(4) == 1)
                    {
                        ballSpeed = FAST;
                        ballMovSpeed = 250;
                        paddleSpeed = 3;
                    }
                    else if(detectKeyPress(0) == 1)
                        cursorPos = ONE;
                    else if(detectKeyPress(2) == 1)
                    {
                        mode = MENU;
                        menuMode = ENTRY;
                    }
                    break;
                }
                break;
                case GAMEPLAY:
                    if(gameplayMode == ALWAYS)
                    {
                        ADCIntClear(ADC1_BASE, 3);
                        ADCProcessorTrigger(ADC1_BASE, 3);

                        /* Wait till conversion is complete */
                        while(!ADCIntStatus(ADC1_BASE, 3, false))
                        {
                        }

                        /* Clear the ADC interrupt flag and get the conversion result */
                        ADCIntClear(ADC1_BASE, 3);
                        ADCSequenceDataGet(ADC1_BASE, 3,&ui32ADC0Value);

                        ui8XAxisAvg = 128 - (ui32ADC0Value/32);
                        switch(gameplayChange){
                        case NORMAL:
                            if(ui8XAxisAvg > 100)
                            {
                                if(paddleXPos >= 120)
                                    paddleXPos = 120;
                                else
                                    paddleXPos += paddleSpeed;
                            }
                            else if(ui8XAxisAvg <= 8)
                            {
                                if(paddleXPos <= 8)
                                    paddleXPos = 8;
                                else
                                    paddleXPos -= paddleSpeed;
                            }
                            paddleDisplay();
                            break;
                        case SPECIAL:
                            if(ui8XAxisAvg > 100)
                            {
                                if(paddleXPos >= 110)
                                    paddleXPos = 110;
                                else
                                    paddleXPos += paddleSpeed;
                            }
                            else if(ui8XAxisAvg <= 28)
                            {
                                if(paddleXPos <= 18)
                                    paddleXPos = 18;
                                else
                                    paddleXPos -= paddleSpeed;
                            }
                            paddleSpecialDisplay();
                            break;
                        }
                    }
        }
    }
}

void displayGLCD(void)
{
    while(1)
    {
        Semaphore_pend(displayGLCDSem, BIOS_WAIT_FOREVER);
        switch(mode){
        case MENU:
            switch(menuMode){
            case ENTRY:
                cursorPos = ONE;
                glcd_write(menuScreen);
                mode = MENU;
                displaySmallText("PLAY", 3, 6);
                displaySmallText("INSTRUCTIONS", 5, 2);
                displaySmallText("SETTINGS", 7, 4);
                menuMode = THROUGHOUT;
                startBeep();
                break;
            case THROUGHOUT:
                cursorDisplay();
                break;
            }
            break;
            case INSTRUCTIONS:
                displaySmallText("USE THUMB STICK TO MOVE PADDLE", 0, 0);
                displaySmallText("YOU HAVE THREE  LIVES", 3, 0);
                displaySmallText("CLEAR ALL BRICKS", 6, 0);
                displaySmallText("ALL THE BEST!", 7, 0);
                break;
            case SETTINGS:
                displaySmallText("DIFFICULTY", 3, 2);
                switch(difficulty){
                case EASY:
                    displaySmallText("EASY  ", 4, 9);
                    lifeCount = 3;
                    break;
                case MEDIUM:
                    displaySmallText("HARD", 4, 9);
                    lifeCount = 2;
                    break;
                case HARD:
                    displaySmallText("INSANE", 4, 9);
                    lifeCount = 1;
                    break;
                }
                displaySmallText("BALL SPEED", 5, 2);
                switch(ballSpeed){
                case SLOW:
                    displaySmallText("SLOW  ", 6, 9);
                    break;
                case FAST:
                    displaySmallText("FAST  ", 6, 9);
                    break;
                case FASTER:
                    displaySmallText("EXTREME", 6, 9);
                    break;
                }
                cursorDisplay();
                break;
                case GAMEPLAY:
                    switch(gameplayMode){
                    case ENTER:
                        score_str[0] = '0';
                        score_str[1] = '0';
                        score_str[2] = '0';
                        paddleXPos = 64;
                        paddleXPrev = paddleXPos;
                        gameplayChange =NORMAL;
                        switch(difficulty){
                        case EASY:
                            screenFlash(threeLivesScreen);
                            break;
                        case MEDIUM:
                            screenFlash(twoLivesScreen);
                            break;
                        case HARD:
                            screenFlash(oneLifeScreen);
                            break;
                        }
                        gameScreen();
                        ballXPos = 7;
                        ballXPrev = 7;
                        ballYPos = 7;
                        ballYPrev = 7;
                        gameplayMode = ALWAYS;
                        break;
                        case DEATH:
                            lifeCount--;
                            if(lifeCount == 2)
                            {
                                screenFlash(twoLivesScreen);
                                gameplayMode = REENTER;
                            }
                            else if(lifeCount == 1)
                            {
                                screenFlash(oneLifeScreen);
                                gameplayMode = REENTER;
                            }
                            else if(lifeCount == 0)
                                mode = GAMEOVER;
                            break;
                        case REENTER:
                            gameScreenRefresh();
                            ballXPos = 7;
                            ballXPrev = 7;
                            ballYPos = 7;
                            ballYPrev = 7;
                            direction = UP_LEFT;
                            gameplayMode = ALWAYS;
                            break;
                        case ALWAYS:
                            ballMovement();
                            if(victoryCheck == 1)
                                mode = VICTORY;
                            ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x20);
                            ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0x04);
                            ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x02);
                            ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x08);
                            break;
                    }
                    break;
                    case VICTORY:
                        victoryAnimationDisplay();
                        play_GOT();
                        glcd_write(creditsScreen);
                        millis(5000);
                        switch(difficulty){
                        case EASY:
                            lifeCount = 3;
                            break;
                        case MEDIUM:
                            lifeCount = 2;
                            break;
                        case HARD:
                            lifeCount = 1;
                            break;
                        }
                        mode = MENU;
                        menuMode = ENTRY;
                        break;
                    case GAMEOVER:
                        direction = INIT;
                        glcd_write(gameOverScreen);
                        displaySmallText(score_str, 5, 9);
                        gameOverMusic();
                        scoreInt = 0;
                        menuMode = ENTRY;
                        mode = MENU;
                        switch(difficulty){
                        case EASY:
                            lifeCount = 3;
                            break;
                        case MEDIUM:
                            lifeCount = 2;
                            break;
                        case HARD:
                            lifeCount = 1;
                            break;
                        }
                        millis(2000);
                        glcd_write(creditsScreen);
                        millis(5000);
                        break;
        }
    }
}

int main(void) {

    latency = 5;

    _init_();
    glcd_init();
    glcd_clearDisplay();
    srand(time(NULL));

    victoryAnimationDisplay();
    play_GOT();
    glcd_write(titleScreen);
    play_MarioBros();

    BIOS_start();

    return(0);
}

void Timer1_ISR(void)
{
    ROM_TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    specialTimeCount++;
    if(specialTimeCount > 10000)
    {
        specialTimeCount = 0;
        gameplayChange = NORMAL;
        glcd_clearRow(7);
        specialExitBeep();
        ROM_TimerDisable(TIMER1_BASE, TIMER_A);
        ROM_IntDisable(INT_TIMER1A);
        ROM_TimerIntDisable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    }
}
void Timer2_ISR(void)
{
    ROM_TimerIntClear(TIMER2_BASE, TIMER_TIMA_TIMEOUT);
    tickCount++;
    switch(tickCount){
    case 10:
        Semaphore_post(displayGLCDSem);
        break;
    case 40:
        Semaphore_post(readInputSem);
        tickCount=0;
        break;
    }
}
