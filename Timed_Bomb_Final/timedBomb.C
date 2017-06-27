/*

 * Author: Umang Satish Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Edited by: Akshay U Hegde

 * Description: This is a Timed Bomb Abstraction using State Machine principles. The Bomb is first intialized
                with a home screen display. The time can be set using switch presses. The timer counts down
                once it is armed. Now, it can be diffused with a key combination. If it is diffused, diffused
                screen is displayed, else, bomb explodes and the system is reset.

 * Filename: timedBomb.c

 * Functions: bombDiffusedScreen(), bombExplosionScreen(), eval_time(), Timer0IntHandler(), updateGLCD(),
              Timer1IntHandler(), KeyPress0(), statemachine0(), exitaction0(), KeyPress1(), statemachine1(),
              exitaction1(), KeyPress2(), statemachine2(), exitaction2(), KeyPress3(), statemachine3(),
              exitaction3(), selectmode(), main()

 * Global Variables: mode, sw1_status, sw2_status, sw3_status, sw4_status, flag1, flag2, flag3, flag4, flag5, flag6, flag7,
                    passbit1, passbit2, passbit3, bit_pos0, bit_pos1, bit_pos2, bit_pos3, currentstate0, currentstate1,
                    currentstate2, currentstate3

 */
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/interrupt.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"

#include "Console/setupConsole.h"
#include "Console/glcdFunctions.h"
#include "Console/glcdLibrary.h"

uint8_t volatile mode;

/*
 * sw1_status=DOWN,sw2_status=UP,sw3_status=LEFT,sw4_status=RIGHT
 * password bits
 * passbit1=down,passbit2=up,passbit3=left
 */

uint32_t volatile sw1_status,sw2_status,sw3_status,sw4_status;
uint8_t volatile flag1=0,flag3=0,flag4=0,flag5=0,flag6=0,flag7=0,passbit1=0,passbit2=0,passbit3=0;
/*
 * Enumeration of States for Each Switch(for debouncing)
 */
enum states{S_idle0,S_press0,S_release0};
enum events{E_npress0,E_press0};
enum states1{S_idle1,S_press1,S_release1};
enum events1{E_npress1,E_press1};
enum states2{S_idle2,S_press2,S_release2};
enum events2{E_npress2,E_press2};
enum states3{S_idle3,S_press3,S_release3};
enum events3{E_npress3,E_press3};
/*
 * Enumeration of states for the Bomb(for State Machine)
 */
enum modes{idle,setTimer,autoTimer,bombExplode,bombDiffused};
// State Initialization
uint8_t volatile currentstate0=S_idle0;
uint8_t volatile currentstate1=S_idle1;
uint8_t volatile currentstate2=S_idle2;
uint8_t volatile currentstate3=S_idle3;
/*
 * Timer Digit Display positions
 */
int bit_pos0=0,bit_pos1=2,bit_pos2=0,bit_pos3=0;
/*
 * Time Counter
 */
int cal_time=120;

/*
 * Function prototyping
 */
int KeyPress0(void);
int KeyPress1(void);
int KeyPress2(void);
int KeyPress3(void);
void statemachine0(int);
void statemachine1(int);
void statemachine2(int);
void statemachine3(int);
void exitaction0(int);
void exitaction1(int);
void exitaction2(int);
void exitaction3(int);
void enteraction0(int);
void enteraction1(int);
void enteraction2(int);
void enteraction3(int);
void selectmode(void);

/*
  * Function Name: glcd_smiley()

 * Input: none

 * Output: none

 * Description: Displays the smiley animation in mode bombDiffused. Reset the password bits and change the mode to idle

 * Example Call: glcd_smiley();
 */
void bombDiffusedScreen(void)
{
    int i,j,k=0;
    // Disable Timer
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerDisable(TIMER1_BASE, TIMER_A);
    IntMasterDisable();

    glcd_cleardisplay();
    // Display Smiley
    for(i=2;i<6;i++)
    {
        for(j=48;j<80;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(smiley[k]);
            k++;
        }
    }
    SysCtlDelay(10000000);
    glcd_cleardisplay();
    // Restart Timer
    IntMasterEnable();
    TimerEnable (TIMER0_BASE, TIMER_A);
    TimerEnable (TIMER1_BASE, TIMER_A);

}
/*

* Function Name: bombExplosionScreen()

* Input: none

* Output: none

* Description: Displays the explosion animation and change the mode back to idle

* Example Call: bombExplosionScreen();

*/
void bombExplosionScreen(void)
{
    int i,j,k=0,l=0;
    // Stop Timer
    IntMasterDisable();
    TimerDisable(TIMER0_BASE, TIMER_A);
    TimerDisable(TIMER1_BASE, TIMER_A);
    while(k<4)
    {
      // Display Explosion Animation(4 screen animation)
        for(i=0;i<8;i++)
        {
            for(j=0;j<128;j++)
            {
                glcd_set_page_col(i,j);
                glcd_data(explosion[k][l]);
                l++;
            }
        }
        l=0;
        glcd_cleardisplay();
        k++;
    }
    glcd_cleardisplay();
    k=0;
    for(i=2;i<6;i++)
    {
      // Display Skull(on Death)
        for(j=48;j<80;j++)
        {
            glcd_set_page_col(i,j);
            glcd_data(skull[k]);
            k++;
        }
    }
    SysCtlDelay(10000000);
    glcd_cleardisplay();
    // Restart Timer
    IntMasterEnable();
    TimerEnable (TIMER0_BASE, TIMER_A);
    TimerEnable (TIMER1_BASE, TIMER_A);

}
/*
 * Function Name: eval_time()

 * Input: none

 * Output: none

 * Description: Evaluate Time for Timer in 4 digit format

 * Example Call: eval_time();
 */

void eval_time()
{
    bit_pos0=0;
    bit_pos1=cal_time/60;
    bit_pos3=(cal_time%60)%10;
    bit_pos2=(cal_time%60)/10;

}

/*

* Function Name: Timer0IntHandler()

* Input: none

* Output: none

* Description: ISR to be called after every Timer0 Interrupt.

* Example Call: Timer0IntHandler();

*/
void Timer0IntHandler(void)
{
// Clear the timer interrupt
TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

switch(mode){
  case setTimer:
      glcd_frame2_write();
      glcd_frame3_write();
      break;
  case autoTimer:
   // In autoTimer Mode, evaluate time and display the timer bits
      glcd_digit_write(bit_pos0,0);
      glcd_digit_write(bit_pos1,1);
      glcd_digit_write(bit_pos2,2);
      glcd_digit_write(bit_pos3,3);
      clear_section_glcd(2,0,78);
      glcd_bomb_write();
  // Switch to Bomb Explosion on Timeout
      if(cal_time==0)
      {
        flag5=1;
        flag4=0;
      }
  // Decrement Time
      if(cal_time>0)
      {
        cal_time--;
      }
      eval_time();
      break;
}
}
/*
 * Function Name: updateGLCD()

* Input: none

* Output: none

* Description: Update GLCD depending upon the mode selected

* Example Call: updateGLCD();
 */

void updateGLCD(void)
{
   //select the mode
    selectmode();
    switch(mode){
      case idle:
          if(flag1 == 0)
          {
            // Display first time once
              cal_time=120;
              glcd_frame1_write();
              flag1=1;
          }
          break;
      case setTimer:
      // In setTimer mode display the timer
      // Display set time with continuous refresh
      glcd_digit_write(bit_pos0,0);
      glcd_digit_write(bit_pos1,1);
      glcd_digit_write(bit_pos2,2);
      glcd_digit_write(bit_pos3,3);
    }
}

/*
 * Function Name: Timer1IntHandler()

* Input: none

* Output: none

* Description: Timer 1 Interupt Handler used for switch debouncing.

* Example Call: Timer1IntHandler();
*/

void Timer1IntHandler(void)
{
  // Clear raised Timer 1 Interrupt
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    statemachine0(KeyPress0());
    statemachine1(KeyPress1());
    statemachine2(KeyPress2());
    statemachine3(KeyPress3());

     updateGLCD();
     switch(mode){
       case(bombExplode):
       // Timer Timeout
          bombExplosionScreen();
          passbit1=0;
          passbit2=0;
          passbit3=0;
          flag5=0;
          flag6=1;
          flag1=0;
          break;
       case(bombDiffused):
       //Passcode Entered
          bombDiffusedScreen();
          passbit1=0;
          passbit2=0;
          passbit3=0;
          flag6=1;
          flag5=0;
          flag1=0;
          flag4=0;
          break;
     }
}
/*
 * Function Name: KeyPress0()

* Input: None

* Output: E_press0/E_npress0

* Description: Read the key press for Switch F4, and return the value.

* Example Call: KeyPress0()
*/

int KeyPress0(void)
{
    sw1_status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4);
    if ((sw1_status)!=(GPIO_PIN_4)) // switch pressed down key
    {
         return E_press0;
    }
    else if((sw1_status)==(GPIO_PIN_4))
    {
        return E_npress0;
    }

}
/*
 * Function Name: Statemachine0(int)

* Input: int

* Output: none

* Description: statemachine for keydebouncing of switch F4

* Example Call: statemachine0(KeyPress0());
*/
void statemachine0(int event)
{
    int nextstate0=currentstate0;
    switch(currentstate0)
    {
    case S_idle0:
        switch(event)
        {
        case E_press0:
            nextstate0=S_press0;
            break;
        case E_npress0:
            nextstate0=S_idle0;
            break;
        }
        break;
    case S_press0:
        switch(event)
        {
        case E_press0:
             nextstate0=S_release0;
             break;
        case E_npress0:
             nextstate0=S_idle0;
             break;
        }
        break;
    case S_release0:
        switch(event)
        {
        case E_press0:
            nextstate0=S_release0;
            break;
        case E_npress0:
            nextstate0=S_idle0;
        }
        break;
        default:
            break;
    }
    if((currentstate0)!=(nextstate0))
    {
        if(!((currentstate0==S_press0)&&(nextstate0==S_idle0)))
        {
        exitaction0(currentstate0);
        currentstate0=nextstate0;
        }
    }
}

/*
 * Function Name: exitaction0(int)

* Input: state

* Output: none

* Description: action to be performed while leaving any state

* Example Call: exitaction0(S_idle0);
*/
void exitaction0(int state)
{
    if(state == S_release0)
    {
      if(mode==setTimer)
      {
          if(cal_time<=10)
          {
              cal_time=120;

          }
          else
          {
          cal_time--;
          }
          eval_time();
          updateGLCD();

      }
      if((mode==autoTimer)&&(passbit2==1))
      {
          passbit1=0;
          passbit2=0;
          passbit3=0;
      }
      if(mode==autoTimer)
      {
          passbit1=1;
      }
    }
}
/*
 * Function Name: KeyPress1()

* Input: none

* Output: E_press1, E_npress1

* Description: read the key press for switch C7 and return the value

* Example Call: statemachine1(KeyPress1());
*/
int KeyPress1(void)
{
    sw2_status=GPIOPinRead(GPIO_PORTC_BASE,GPIO_PIN_7);
    if ((sw2_status)!=(GPIO_PIN_7)) // switch pressed up
    {
         return E_press1;
    }
    else if((sw2_status)==(GPIO_PIN_7))
    {
        return E_npress1;
    }

}
/*
 * Function Name: Statemachine1(int)

* Input: event

* Output: none

* Description: statemachine for keydebouncing of switch C7

* Example Call: statemachine1(KeyPress1());
*/
void statemachine1(int event)
{
    int nextstate1=currentstate1;
    switch(currentstate1)
    {
    case S_idle1:
        switch(event)
        {
        case E_press1:
            nextstate1=S_press1;
            break;
        case E_npress1:
            nextstate1=S_idle1;
            break;
        }
        break;
    case S_press1:
        switch(event)
        {
        case E_press1:
             nextstate1=S_release1;
             break;
        case E_npress1:
             nextstate1=S_idle1;
             break;
        }
        break;
    case S_release1:
        switch(event)
        {
        case E_press1:
            nextstate1=S_release1;
            break;
        case E_npress1:
            nextstate1=S_idle1;
        }
        break;
        default:
            break;
    }
    if((currentstate1)!=(nextstate1))
    {
        if(!((currentstate1==S_press1)&&(nextstate1==S_idle1)))
        {
        exitaction1(currentstate1);
        currentstate1=nextstate1;
        }
    }

}
/*
 * Function Name: exitaction1(int)

* Input: int

* Output: none

* Description: action to be performed while leaving any state

* Example Call: exitaction1(currentstate1);
*/
void exitaction1(int state)
{
    if(state == S_release1)
    {
      if(mode==setTimer)
      {
          if(cal_time>=120)
          {
              cal_time=10;

          }
          else
          {
          cal_time++;
          }
          eval_time();
          updateGLCD();

      }
      if((mode==autoTimer)&&(passbit1==1))
      {
          passbit2=1;
      }
    }
}

/*
 * Function Name: KeyPress2()

* Input: none

* Output: int

* Description: read the key press for switch D6 and return the value

* Example Call: statemachine2(KeyPress2());
*/
int KeyPress2(void)
{
    sw3_status=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_6);
    if ((sw3_status)!=(GPIO_PIN_6)) // switch pressed
    {
         return E_press2;
    }
    else if((sw3_status)==(GPIO_PIN_6))
    {
        return E_npress2;
    }

}
/*
 * Function Name: Statemachine2(int)

* Input: int

* Output: none

* Description: statemachine for keydebouncing of switch D6

* Example Call: statemachine2(KeyPress2());
*/
void statemachine2(int event)
{
    int nextstate2=currentstate2;
    switch(currentstate2)
    {
    case S_idle2:
        switch(event)
        {
        case E_press2:
            nextstate2=S_press2;
            break;
        case E_npress2:
            nextstate2=S_idle2;
            break;
        }
        break;
    case S_press2:
        switch(event)
        {
        case E_press2:
             nextstate2=S_release2;
             break;
        case E_npress2:
             nextstate2=S_idle2;
             break;
        }
        break;
    case S_release2:
        switch(event)
        {
        case E_press2:
            nextstate2=S_release2;
            break;
        case E_npress2:
            nextstate2=S_idle2;
        }
        break;
        default:
            break;
    }
    if((currentstate2)!=(nextstate2))
    {
        if(!((currentstate2==S_press2)&&(nextstate2==S_idle2)))
        {
        exitaction2(currentstate2);
        currentstate2=nextstate2;
        }
    }
}
/*
 * Function Name: exitaction2(int)

* Input: state

* Output: none

* Description: action to be performed while leaving any state

* Example Call: exitaction2(S_idle2);
*/
void exitaction2(int state)
{
    if(state == S_release2)
    {
      if(mode == idle)
      {
          flag3=1;
          glcd_cleardisplay();
          eval_time();
          glcd_digit_write(bit_pos0,0);
          glcd_digit_write(bit_pos1,1);
          glcd_digit_write(bit_pos2,2);
          glcd_digit_write(bit_pos3,3);

      }
      if((mode == autoTimer)&&(passbit2 == 1))
      {
          passbit3=1;
      }
      else if((mode == autoTimer)&&(passbit1 == 1))
      {
          passbit1=0;
          passbit2=0;
          passbit3=0;
      }
    }
}
/*
 * Function Name: KeyPress3()

* Input: none

* Output: E_press3, E_npress3

* Description: read the key press for switch D7 and return the value

* Example Call: KeyPress3();
*/
int KeyPress3(void)
{
    sw4_status=GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_7);
    if ((sw4_status)!=(GPIO_PIN_7)) // switch pressed
    {
         return E_press3;
    }
    else if((sw4_status)==(GPIO_PIN_7))
    {
        return E_npress3;
    }

}
/*
 * Function Name: Statemachine3(int)

* Input: event

* Output: none

* Description: statemachine for keydebouncing of switch D7

* Example Call: statemachine3(KeyPress3());
*/
void statemachine3(int event)
{
    int nextstate3=currentstate3;
    switch(currentstate3)
    {
    case S_idle3:
        switch(event)
        {
        case E_press3:
            nextstate3=S_press3;
            break;
        case E_npress3:
            nextstate3=S_idle3;
            break;
        }
        break;
    case S_press3:
        switch(event)
        {
        case E_press3:
             nextstate3=S_release3;
             break;
        case E_npress3:
             nextstate3=S_idle3;
             break;
        }
        break;
    case S_release3:
        switch(event)
        {
        case E_press3:
            nextstate3=S_release3;
            break;
        case E_npress3:
            nextstate3=S_idle3;
        }
        break;
        default:
            break;
    }
    if((currentstate3)!=(nextstate3))
    {
        if(!((currentstate3==S_press3)&&(nextstate3==S_idle3)))
        {
        exitaction3(currentstate3);

        currentstate3=nextstate3;
        }
    }
}
/*
 * Function Name: exitaction3(int)

* Input: int

* Output: none

* Description: action to be performed while leaving any state

* Example Call: exitaction3(currentstate3);
*/
void exitaction3(int state)
{
    if(state == S_release3)
    {
      if(mode==setTimer)
      {
      flag4=1;
      flag3=0;
      }
      if((mode==autoTimer))
      {
          passbit1=0;
          passbit2=0;
          passbit3=0;
      }
    }
}
/*
 * Function Name: selectmode()

* Input: none

* Output: none

* Description: Select mode depending upon the status of flags

* Example Call: selectmode();
*/

void selectmode(void)
{
/*
 * if password bit3 is 1 then mode is bombDiffused
 */
    if(passbit3==1)
    {
        mode=bombDiffused;
    }
    /*
     * If flag3 is set then mode is setTimer
     */
    else if(flag3==1)
   {
       mode=setTimer;
   }
    /*
     * If flag4=1 is set then mode is autoTimer
     */
   else if(flag4==1)
   {
       mode=autoTimer;
   }
    /*
     * If flag5 is set then mode is bombExplode
     */
   else if(flag5==1)
   {
       mode=bombExplode;
   }
    /*
     * if flag6 is set then mode is idle
     */
   else if(flag6==1)
   {
       mode=idle;
   }

}
/*
 * Function Name: main()

* Input: none

* Output: none

* Description: Performs initialization, and performs mode selection continuously

* Example Call: main();
*/
void main()
{

    setup();
    glcd_init();
    glcd_cleardisplay();
    /*
     * enable the timer 0 and 1
     */
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntEnable(INT_TIMER1A);
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();

    TimerEnable (TIMER0_BASE, TIMER_A);
    TimerEnable (TIMER1_BASE, TIMER_A);
    /*
     * initial mode is idle
     */
    mode=idle;

    while(1)
    {
       /*
        * select mode continuously
        */
        selectmode();

    }

}
