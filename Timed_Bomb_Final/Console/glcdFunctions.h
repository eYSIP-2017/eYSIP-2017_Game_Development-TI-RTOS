/*

 * Author: Umang Deshpande, Intern, eYSIP - 2017, IIT Bombay.

 * Edited by: Akshay U Hegde

 * Description: This is the header file for working with JHD12864 LCD on the console.

 * Filename: glcdFunctions.h
 */

#ifndef CONSOLE_GLCDFUNCTIONS_H_
#define CONSOLE_GLCDFUNCTIONS_H_

extern void glcd_data(char data);
extern void glcd_cmd(unsigned char cmd);
extern void glcd_init(void);
extern void glcd_set_page_col(int page,int col);
extern void glcd_cleardisplay(void);
extern void glcd_img_write(unsigned char img[]);
extern void glcd_bomb_write();
extern void glcd_digit_write(int number, int position);
extern void glcd_frame2_write(void);
extern void glcd_frame1_write(void);
extern void glcd_frame3_write(void);
extern void clear_section_glcd(int a,int b,int c);

#endif /* CONSOLE_GLCDFUNCTIONS_H_ */
