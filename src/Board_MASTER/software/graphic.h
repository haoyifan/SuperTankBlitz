/*
 * graphic.h
 *
 *  Created on: Mar 21, 2016
 *      Author: haoyifan
 */

#ifndef GRAPHIC_H_
#define GRAPHIC_H_

#include <inttypes.h>
#include <stdio.h>
#include "drivers/mss_uart/mss_uart.h"

/*
 * wrapper for the UART1 initialization
 */
void graphic_init();

/*
 * clear the screen
 */
void clear_screen();

/*
 * set the x coordinate. 0x00 is the left side
 */
void set_x_coordinate(uint8_t x_coor);

/*
 * set the y coordinate. 0x80 is the top
 */
void set_y_coordinate(uint8_t y_coor);

/*
 * draw straight line between two pixels
 */
void draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t draw);

/*
 * toggle the pixel
 */
void set_pixel(uint8_t x_coor, uint8_t y_coor, uint8_t set);

/*
 * draw or erase a circle
 * 1 means draw; 0 means erase
 */
void draw_circle(uint8_t x_cen, uint8_t y_cen, uint8_t radius, uint8_t draw);

/*
 * draw box that is filled from one corner to another corner
 * draw means if draw or erase
 */
void draw_box(uint8_t x_leftup, uint8_t y_leftup, uint8_t x_rightdown, uint8_t y_rightdown, uint8_t draw);

/*
 * erase the content of the box
 */
void erase_block(uint8_t x_leftup, uint8_t y_leftup, uint8_t x_rightdown, uint8_t y_rightdown);

/*
 * diaplay the context
 */
void write(uint8_t context[]);

#endif /* GRAPHIC_H_ */
