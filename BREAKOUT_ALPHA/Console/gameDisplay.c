/*
 * gameDisplay.c
 *
 *  Created on: 24-Jun-2017
 *      Author: Akshay U Hegde
 */

#include <Objects/gameObjects.h>
#include <stdint.h>
#include <stdbool.h>
#include <Screens/gameScreens.h>

#include "consoleInit.h"
#include "glcd.h"
#include "tones.h"

#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"

#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"

signed int paddleXPos, paddleXPrev;
signed char hit[40];
unsigned char ballXPos, ballYPos, ballXPrev, ballYPrev;
unsigned int scoreInt;
char score_str[4];

enum cursorPositions
{
    ONE,
    TWO,
    THREE
};

enum cursorPositions cursorPos;

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

enum gameplayChanges
{
    NORMAL,
    SPECIAL
};

enum gameplayChanges gameplayChange;

void ballDisplay(unsigned char *image)
{
    unsigned char i = 0, j = 0;
    if(((ballXPrev == ballXPos) && (ballYPrev == ballYPos)) != 1)
    {
        glcd_setPage(ballYPrev);
        for(i = (ballXPrev*8); i < ((ballXPrev*8) + 8); i++)
        {
            // Select all the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(0x00);
            j++;
        }
    }
    glcd_setPage(ballYPos);
    for(i = (ballXPos*8); i < ((ballXPos*8) + 8); i++)
    {
        // Select all the columns
        glcd_setColumn(i);

        //Send hex values GLCD
        glcd_data(image[j]);
        j++;
    }
    ballXPrev = ballXPos;
    ballYPrev = ballYPos;
}

void paddleDisplay(void)
{
    unsigned char i = 0, j =0;
    glcd_setPage(7);
    if(paddleXPrev != paddleXPos)
    {
        for(i = (paddleXPrev); i < (paddleXPrev + 16); i++)
        {
            // Select all the columns
            glcd_setColumn(i-8);

            //Send hex values GLCD
            glcd_data(0x00);
        }
    }
    for(i = (paddleXPos); i < (paddleXPos + 16); i++)
    {
        // Select all the columns
        glcd_setColumn(i-8);

        //Send hex values GLCD
        glcd_data(paddle[j]);
        j++;
    }
    paddleXPrev = paddleXPos;
}
void paddleSpecialDisplay(void)
{
    unsigned char i = 0, j =0;
    glcd_setPage(7);
    if(paddleXPrev != paddleXPos)
    {
        for(i = (paddleXPrev); i < (paddleXPrev + 32); i++)
        {
            // Select all the columns
            glcd_setColumn(i-16);

            //Send hex values GLCD
            glcd_data(0x00);
        }
    }
    for(i = (paddleXPos); i < (paddleXPos + 32); i++)
    {
        // Select all the columns
        glcd_setColumn(i-16);

        //Send hex values GLCD
        glcd_data(paddleSpecial[j]);
        j++;
    }
    paddleXPrev = paddleXPos;
}
void victoryAnimationDisplay(void)
{
    glcd_write(victoryScreen1);
    millis(400);
    glcd_clearDisplay();
    glcd_write(victoryScreen2);
    millis(400);
    glcd_clearDisplay();
    glcd_write(victoryScreen3);
    millis(400);
    glcd_clearDisplay();
    glcd_write(victoryScreen4);
    millis(400);
}
void cursorDisplay(void)
{
    switch(cursorPos){
    case ONE:
        displaySmallText(" ", 5, 1);
        displaySmallText(" ", 7, 1);
        displaySmallText("*", 3, 1);
        break;
    case TWO:
        displaySmallText(" ", 3, 1);
        displaySmallText(" ", 7, 1);
        displaySmallText("*", 5, 1);
        break;
    case THREE:
        displaySmallText(" ", 3, 1);
        displaySmallText(" ", 5, 1);
        displaySmallText("*", 7, 1);
        break;
    }
}
void blockDisplay(unsigned char blockYPos, unsigned char blockXPos, unsigned char *image)
{
    unsigned char i = 0, j = 0;
    glcd_setPage(blockYPos);

    for(i = ((blockXPos*12) + 4); i < ((blockXPos*12) + 16); i++)
    {
        // Select all the columns
        glcd_setColumn(i);

        //Send hex values GLCD
        glcd_data(image[j]);
        j++;
    }
}
void blockClear(unsigned char blockXPos, unsigned char blockYPos)
{
    scoreInt++;
    score_str[2] = (scoreInt%10) + '0';
    score_str[1] = (scoreInt/10) + '0';
    if(scoreInt > 100)
    {
        score_str[0] = (scoreInt/100) + '0';
        score_str[1] = ((scoreInt%100)%10) + '0';
    }
    unsigned char i = 0;
    hit[((blockYPos*10) + blockXPos)]--;
    switch(hit[((blockYPos*10) + blockXPos)]){
    case 0:
        glcd_setPage(blockYPos);
        for(i = ((blockXPos*12) + 4); i < ((blockXPos*12) + 16); i++)
        {
            // Select all the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(0x00);
        }
        blockBeep();
        if(block[blockXPos + (10*blockYPos)] == BLOCK_MAGIC1)
        {
            ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x00);
            blockClearSurround(blockXPos, blockYPos);
            block[((blockYPos*10) + blockXPos)] = NONE;
            break;
        }
        else if(block[blockXPos + (10*blockYPos)] == BLOCK_MAGIC2)
        {
            ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0x00);
            gameplayChange = SPECIAL;
            specialEntryBeep();
            ROM_TimerEnable(TIMER1_BASE, TIMER_A);
            ROM_IntEnable(INT_TIMER1A);
            ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
            block[((blockYPos*10) + blockXPos)] = NONE;
            break;
        }
        else
        {
            ROM_GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0x00);
            millis(50);
            block[((blockYPos*10) + blockXPos)] = NONE;
            break;
        }
    case 1:
        ROM_GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_5, 0x00);
        blockDisplay(blockYPos, blockXPos, block_easy);
        blockBeep();
        block[((blockYPos*10) + blockXPos)] = BLOCK_EASY;
        break;
    case 2:
        ROM_GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0x00);
        blockDisplay(blockYPos, blockXPos, block_med);
        blockBeep();
        block[((blockYPos*10) + blockXPos)] = BLOCK_MED;
        break;
    default:
        break;
    }
}
void blockClearSurround(unsigned char blockX, unsigned char blockY)
{
    unsigned char flagXL,flagYT,flagXR;
    if(blockX != 0)
    {
        blockClear(blockX-1,blockY);
        flagXL = 1;
    }
    if(blockY != 0)
    {
        blockClear(blockX, blockY-1);
        flagYT = 1;
        if(flagXL == 1)
            blockClear(blockX-1,blockY-1);
    }
    if(blockX != 9)
    {
        blockClear(blockX+1,blockY);
        flagXR = 1;
        if(flagYT == 1)
            blockClear(blockX+1, blockY-1);
    }
    if(blockY != 3)
    {
        blockClear(blockX, blockY+1);
        if(flagXR == 1)
            blockClear(blockX+1,blockY+1);
        if(flagXL == 1)
            blockClear(blockX-1,blockY+1);
    }
}
void screenFlash(unsigned char *image)
{
    unsigned char flashCount;
    for(flashCount = 0; flashCount < 3; flashCount++)
    {
        glcd_write(image);
        millis(500);
        glcd_clearDisplay();
        millis(500);
        glcd_write(image);
    }
}
