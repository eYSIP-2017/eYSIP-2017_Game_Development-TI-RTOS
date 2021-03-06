/*
 * glcd_init.h
 *
 *  Created on: 30-May-2017
 *      Author: AxRox
 */

#ifndef GLCD_H_
#define GLCD_H_

extern void glcd_data(unsigned char data);
extern void glcd_cmd(unsigned char cmd);
extern void glcd_setColumn(unsigned char column);
extern void glcd_setPage(unsigned char page);
extern void glcd_cleanup(unsigned char page, unsigned char quadrant);
extern void glcd_clearDisplay(void);
extern void glcd_write(unsigned char *image);
extern void glcd_draw(unsigned char x_pos, unsigned char y_pos);
extern void glcd_init(void);

#endif /* GLCD_H_ */
