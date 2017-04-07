/*
 * graphic.c
 *
 *  Created on: Mar 21, 2016
 *      Author: haoyifan
 */

#include <inttypes.h>
#include <stdio.h>
#include "graphic.h"
#include "drivers/mss_uart/mss_uart.h"

//---------------------------------------tested-----------------------------------------------
void graphic_init()
{
	MSS_UART_init
	 (
		 &g_mss_uart1,
		 MSS_UART_115200_BAUD,
		 MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
	 );
}

//---------------------------------------tested-----------------------------------------------
void clear_screen()
{
	uint8_t message[] = {0x7C, 0x00};
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//---------------------------------------tested-----------------------------------------------
void set_x_coordinate(uint8_t x_coor)
{
	uint8_t message[] = {0x7C, 0x18, 0x00};
	message[2] = x_coor;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//---------------------------------------tested-----------------------------------------------
void set_y_coordinate(uint8_t y_coor)
{
	uint8_t message[] = {0x7C, 0x19, 0x80};
	message[2] = y_coor;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//---------------------------------------tested-----------------------------------------------
void draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t draw)
{
	uint8_t message[] = {0x7C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
	message[2] = x_start;
	message[3] = y_start;
	message[4] = x_end;
	message[5] = y_end;
	message[6] = draw;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//---------------------------------------tested-----------------------------------------------
void set_pixel(uint8_t x_coor, uint8_t y_coor, uint8_t set)
{
	uint8_t message[] = {0x7C, 0x10, 0x00, 0x00, 0x00};
	message[2] = x_coor;
	message[3] = y_coor;
	message[4] = set;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

//---------------------------------------tested-----------------------------------------------
void draw_circle(uint8_t x_cen, uint8_t y_cen, uint8_t radius, uint8_t draw)
{
	uint8_t message[] = {0x7C, 0x03, 0x00, 0x00, 0x00, 0x00};
	message[2] = x_cen;
	message[3] = y_cen;
	message[4] = radius;
	message[5] = draw;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

// remain to be tested
void draw_box(uint8_t x_leftup, uint8_t y_leftup, uint8_t x_rightdown, uint8_t y_rightdown, uint8_t draw)
{
	uint8_t message[] = {0x7C, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00};
	message[2] = x_leftup;
	message[3] = y_leftup;
	message[4] = x_rightdown;
	message[5] = y_rightdown;
	message[6] = draw;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

// remain to be tested
void erase_block(uint8_t x_leftup, uint8_t y_leftup, uint8_t x_rightdown, uint8_t y_rightdown)
{
	uint8_t message[] = {0x7C, 0x05, 0x00, 0x00, 0x00, 0x00};
	message[2] = x_leftup;
	message[3] = y_leftup;
	message[4] = x_rightdown;
	message[5] = y_rightdown;
	MSS_UART_polled_tx( &g_mss_uart1, message, sizeof(message) );
}

// remain to be tested
void write(uint8_t context[])
{
	MSS_UART_polled_tx( &g_mss_uart1, context, sizeof(context) );
}
