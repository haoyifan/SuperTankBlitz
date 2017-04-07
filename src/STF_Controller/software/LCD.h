/*
 * LCD.h
 *
 *  Created on: Mar 9, 2016
 *      Author: yangsj
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include "low_level_helper.h"

#ifndef LCD_H_
#define LCD_H_

#define block_char 0xff

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00


void displayHP(uint16_t hp);

void displayReloadLight(uint16_t time);

void displayReloadHeavy(uint16_t time);

void displaySpeeding(uint8_t speeding);

void displayHaveStun(uint8_t healing);

void displayFreezing(uint8_t freezing);


/*
 * clear the screen
 */
void clear();

/*
 * cursor return home
 */
void return_home();

/*
 * set the moving direction of cursor and display
 */
void entry_mode_set(uint8_t ID, uint8_t SH);

/*
 * display on off control
 */
void display_on_off_control(uint8_t D, uint8_t C, uint8_t B);

/*
 * shifting of right left cursor position or display without writing
 * or reading of display data
 */
void cursor_display_shift(uint8_t SC, uint8_t RL);

/*
 * function set
 */
void function_set(uint8_t DL, uint8_t N, uint8_t F);

/*
 * set CGRAM address
 */
void set_CGRAM_addr(uint8_t addr);

/*
 * set DDRAM address
 */
void set_DDRAM_addr(uint8_t addr);

/*
 * write data to RAM
 */
void write_data(uint8_t data);

/*
 * read data from RAM
 */
uint8_t read_data();

/*
 * ta ma de wo ye bu zhi dao zen me she zhi di zhi le
 * the input pos corresponds to the pos in the screen
 */
void set_display_position(uint8_t pos);

/*
 * write a string
 */
void write_string(char* string);
#endif /* LCD_H_ */
