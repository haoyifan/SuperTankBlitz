/*
 * low_level_helper.h
 *
 *  Created on: Mar 9, 2016
 *      Author: yangsj
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "drivers/mss_gpio/mss_gpio.h"
#include <inttypes.h>

#ifndef LOW_LEVEL_HELPER_H_
#define LOW_LEVEL_HELPER_H_

#define LOW 0
#define HIGH 1
#define HIGH_Z 2

#define DB_HIGH_Z 1
#define NO_DB_HIGH_Z 0

#define COMMAND_MODE 0
#define DATA_MODE 1

#define DISPLAY_ON 1
#define DISPLAY_OFF 0

#define CURSOR_ON 1
#define CURSOR_OFF 0

#define BLINK_ON 1
#define BLINK_OFF 0

/*
 * initialize the GPIO pin mode and GPIO initialization
 */
void init_GPIO();

/*
 * init function for LCD
 */
void LCD_init();

/*
 * if_highz is 1, high Z all the DB pins.
 * Else set the output according to value
 */
void DB_mode(uint8_t if_highz, uint8_t value);

/*
 * send a command
 */
void send_command(uint8_t command);

/*
 * send a data
 */
void send_data(uint8_t data);

/*
 * wrapped send function
 * when 0, it sends command
 * when 1, it sends data
 */
void send(uint8_t value, uint8_t mode);

/*
 * set RS value
 * can only be 1 or 0
 */
void set_RS(uint8_t mode);

/*
 * set R/W
 * can only be 1 or 0
 */
void set_R_W(uint8_t mode);

/*
 * delay for 100 cycles
 */
void delay();

/*
 * check if LCD is busy. Return when not busy
 */
void check_if_busy();

/*
 * set enable signal
 */
void set_enable(uint8_t EN);

#endif /* LOW_LEVEL_HELPER_H_ */
