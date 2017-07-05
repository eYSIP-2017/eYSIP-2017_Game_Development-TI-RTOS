/*

 * Author: Akshay U Hegde, Intern, eYSIP - 2017, IIT Bombay.
           Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Description: All the functions which deal with display of various game objects are defined in this header
                including ball, paddle and bricks.

 * Filename: gameDisplay.c

 * Functions: ballDisplay(), paddleDisplay(), paddleSpecialDisplay(), cursorDisplay(), blockDisplay(),
              blockClear(), blockClearSurround()

 * Global Variables: paddleXPos, paddleXPrev, hit[64], ballXPos, ballYPos, ballXPrev, ballYPrev, scoreInt,
                     specialTimeCount, score_str[4]

 */
// Standard C Headers
#include <stdint.h>
#include <stdbool.h>
// Custom Console Header
#include "consoleInit.h"
#include "glcd.h"
#include "tones.h"
#include <Screens/gameScreens.h>
#include <Objects/gameObjects.h>
// Headers for GPIO Functions
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
// ROM Header
#define TARGET_IS_BLIZZARD_RB1
#include "driverlib/rom.h"
/*
 * Global Variable Declaration
 */
signed int paddleXPos, paddleXPrev;
signed char hit[64];
unsigned char ballXPos, ballYPos, ballXPrev, ballYPrev;
unsigned int scoreInt, specialTimeCount;
char score_str[4];
/*
 * State Enumeration
 */
enum cursorPositions
{
    ONE,
    TWO,
    THREE
};
// Initialization
enum cursorPositions cursorPos;
// Block States
enum blocks
{
    NONE,
    BLOCK_HARD,
    BLOCK_MED,
    BLOCK_EASY,
    BLOCK_MAGIC1,
    BLOCK_MAGIC2
};
// Initialization
enum blocks block[80];
// Paddle Size States
enum gameplayChanges
{
    NORMAL,
    SPECIAL
};
// Initialization
enum gameplayChanges gameplayChange;
/*

 * Function Name: ballDisplay()

 * Input: unsigned char *image

 * Output: None

 * Description: This function clears the previous position of the ball, and displays the new position
                of the ball(Can display 8x8 Image, two different images are fed to display 4x4 ball in different quadrants)

 * Example Call: ballMovement(ballInit)

 */
void ballDisplay(unsigned char *image)
{
    unsigned char i = 0, j = 0;
    glcd_setPage(ballYPrev);
    // Clear previously displayed Ball
    if(((ballXPrev == ballXPos) && (ballYPrev == ballYPos)) != 1)
    {
        glcd_setPage(ballYPrev);
        for(i = (ballXPrev*8); i < ((ballXPrev*8) + 8); i++)
        {
            // Select given columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(0x00);
            j++;
        }
    }
    // Display ball at current position
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
/*

 * Function Name: paddleDisplay()

 * Input: None

 * Output: None

 * Description: This function clears the previous position of the paddle of regular size, and displays the new position
                of the regular paddle.

 * Example Call: paddleDisplay()

 */
void paddleDisplay(void)
{
    unsigned char i = 0, j =0;
    // Clear previously displayed paddle
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
    // Display paddle at current position
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
/*

 * Function Name: paddleSpecialDisplay()

 * Input: None

 * Output: None

 * Description: This function clears the previous position of the paddle of larger size, and displays the new position
                of the special paddle.

 * Example Call: paddleSpecialDisplay()

 */
void paddleSpecialDisplay(void)
{
    unsigned char i = 0, j =0;
    glcd_setPage(7);
    // Clear previously displayed paddle
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
    // Display special paddle at current position
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
/*

 * Function Name: cursorDisplay()

 * Input: None

 * Output: None

 * Description: This function displays a cursor at its requisite position on the screen based on its state value.

 * Example Call: cursorDisplay()

 */
void cursorDisplay(void)
{
    switch(cursorPos){
    case ONE:
        // Clear Cursor if any in other positions and display at requisit position
        displaySmallText(" ", 5, 1);
        displaySmallText(" ", 7, 1);
        displaySmallText("*", 3, 1); //* is mapped to cursor
        break;
    case TWO:
        // Clear Cursor if any in other positions and display at requisit position
        displaySmallText(" ", 3, 1);
        displaySmallText(" ", 7, 1);
        displaySmallText("*", 5, 1); //* is mapped to cursor
        break;
    case THREE:
        // Clear Cursor if any in other positions and display at requisit position
        displaySmallText(" ", 3, 1);
        displaySmallText(" ", 5, 1);
        displaySmallText("*", 7, 1); //* is mapped to cursor
        break;
    }
}
void blockDisplay(unsigned char blockYPos, unsigned char blockXPos, unsigned char *image)
{
    unsigned char i = 0, j = 0;
    glcd_setPage(blockYPos);

    for(i = ((blockXPos*16)); i < ((blockXPos+1)*16); i++)
    {
        // Select all the columns
        glcd_setColumn(i);

        //Send hex values GLCD
        glcd_data(image[j]);
        j++;
    }
}
/*

 * Function Name: blockClear()

 * Input: blockXPos, blockYPos

 * Output: None

 * Description: This function delivers a hit to the block at whatever X and Y Block co-ordinates as specified. Depending on
                remaining number of hits and type of brick, different actions are performed. Also calculates the score.

 * Example Call: blockClear(3,2)

 */
void blockClear(unsigned char blockXPos, unsigned char blockYPos)
{
    // Increase Score Count
    scoreInt++;
    // Map Scorecount to a character for display on screen
    score_str[2] = (scoreInt%10) + '0';
    score_str[1] = (scoreInt/10) + '0';
    if(scoreInt > 100)
    {
        // For three digit score
        score_str[0] = (scoreInt/100) + '0';
        score_str[1] = ((scoreInt%100)%10) + '0';
    }
    unsigned char i = 0;
    // Decrease Number of hits of the block
    hit[((blockYPos*8) + blockXPos)]--;
    // Switch case performs different actions based on value of hit
    switch(hit[((blockYPos*8) + blockXPos)]){
    case 0:
        millis(50);
        glcd_setPage(blockYPos);
        // Clear Block
        for(i = (blockXPos*16); i < ((blockXPos+1)*16); i++)
        {
            // Select all the columns
            glcd_setColumn(i);

            //Send hex values GLCD
            glcd_data(0x00);
        }
        // Block Clear Feedback
        blockBeep();
        if(block[blockXPos + (8*blockYPos)] == BLOCK_MAGIC1)
        {
            block[((blockYPos*8) + blockXPos)] = NONE;
            // Clear Surrounding Blocks for BLOCK_MAGIC1 which takes 1 hit
            blockClearSurround(blockXPos, blockYPos);
            break;
        }
        else if(block[blockXPos + (8*blockYPos)] == BLOCK_MAGIC2)
        {
            // Change Paddle Size for BLOCK_MAGIC2 which takes 1 hit
            gameplayChange = SPECIAL;
            // If paddle is on the left corner, reset paddle position to prevent GLCD error
            if(paddleXPos < 18)
                paddleXPos = 18;
            else if(paddleXPos > 120)
                paddleXPos = 120;
            // Start 10 s Timer for the Special Mode Duration
            specialTimeCount = 0;
            specialEntryBeep();
            ROM_TimerEnable(TIMER1_BASE, TIMER_A);
            ROM_IntEnable(INT_TIMER1A);
            ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
            block[((blockYPos*8) + blockXPos)] = NONE;
            break;
        }
        else
        {
            // If the Block isn't a special block and is a BLOCK_EASY, clear it
            millis(50);
            block[((blockYPos*8) + blockXPos)] = NONE;
            break;
        }
    case 1:
        // In case the block is BLOCK_MED
        // Display BLOCK_EASY which has 1 hit
        blockDisplay(blockYPos, blockXPos, block_easyv2);
        blockBeep();
        block[((blockYPos*8) + blockXPos)] = BLOCK_EASY;
        break;
    case 2:
        // In case the block is BLOCK_HARD
        // Display BLOCK_MED which has 2 hits
        blockDisplay(blockYPos, blockXPos, block_medv2);
        blockBeep();
        block[((blockYPos*8) + blockXPos)] = BLOCK_MED;
        break;
    default:
        break;
    }
}
/*

 * Function Name: blockClearSurround()

 * Input: blockX, blockY

 * Output: None

 * Description: This function delivers a hit to all the surrounding blocks of the block whose
                X and Y Block co-ordinates as specified. This is performed only for BLOCK_MAGIC1.

 * Example Call: blockClearSurround(4,5)

 */
void blockClearSurround(unsigned char blockX, unsigned char blockY)
{
    unsigned char flagXL,flagYT,flagXR;
    // Check whether the block is not the leftmost
    if(blockX != 0)
    {
        // Clear Block to the Left
        blockClear(blockX-1,blockY);
        flagXL = 1;
    }
    // Check whether the block is not the topmost
    if(blockY != 0)
    {
        // Clear Upper Block
        blockClear(blockX, blockY-1);
        flagYT = 1;
        // Clear Upper Left Block
        if(flagXL == 1)
            blockClear(blockX-1,blockY-1);
    }
    // Check whether the block is not rightmost
    if(blockX != 9)
    {
        // Clear Block to the Right
        blockClear(blockX+1,blockY);
        flagXR = 1;
        // Clear Upper Right Block
        if(flagYT == 1)
            blockClear(blockX+1, blockY-1);
    }
    // Check whether the block is not bottommost
    if(blockY != 3)
    {
        // Clear Lower Block
        blockClear(blockX, blockY+1);
        // Clear Lower Right Block
        if(flagXR == 1)
            blockClear(blockX+1,blockY+1);
        // Clear Lower Left Block
        if(flagXL == 1)
            blockClear(blockX-1,blockY+1);
    }
}

