/*
 * low_level_helper.c
 *
 *  Created on: Mar 9, 2016
 *      Author: yangsj
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "LCD.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include <inttypes.h>
#include "low_level_helper.h"

/**************************Finish****************************/

void init_GPIO()
{
	MSS_GPIO_init();
	// DB0 - DB7. Defined as inout port
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_2, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_3, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_4, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_5, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_6, MSS_GPIO_INOUT_MODE);
	MSS_GPIO_config( MSS_GPIO_7, MSS_GPIO_INOUT_MODE);

	// need to drive inout as high Z at beginning
	DB_mode(HIGH_Z, 0);

	// RS     0 - instruction       1 - data
	MSS_GPIO_config( MSS_GPIO_8, MSS_GPIO_OUTPUT_MODE);

	// R/W    0 - read              1 - write
	MSS_GPIO_config( MSS_GPIO_9, MSS_GPIO_OUTPUT_MODE);

	// E (enable)
	MSS_GPIO_config( MSS_GPIO_10, MSS_GPIO_OUTPUT_MODE);

	//set initial output
	MSS_GPIO_set_outputs(0x00400);
}

/**************************Finish****************************/

void DB_mode(uint8_t if_highz, uint8_t value)
{
	if(if_highz)
	{
		MSS_GPIO_drive_inout( MSS_GPIO_0, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_1, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_2, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_3, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_4, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_5, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_6, MSS_GPIO_HIGH_Z );
		MSS_GPIO_drive_inout( MSS_GPIO_7, MSS_GPIO_HIGH_Z );
		return;
	}
	else
	{
		uint8_t mask = 1;
		uint8_t mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_0, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_0, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_1, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_1, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_2, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_2, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_3, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_3, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_4, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_4, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_5, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_5, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_6, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_6, MSS_GPIO_DRIVE_LOW );
		}

		mask = mask << 1;
		mask_result = mask & value;
		if(mask_result)
		{
			MSS_GPIO_drive_inout( MSS_GPIO_7, MSS_GPIO_DRIVE_HIGH );
		}
		else
		{
			MSS_GPIO_drive_inout( MSS_GPIO_7, MSS_GPIO_DRIVE_LOW );
		}
	}
}

/**************************Finish****************************/

void LCD_init()
{
	DB_mode(HIGH_Z, 0);

	// display clear
	check_if_busy();
	clear();

	// function set
	check_if_busy();
	function_set(1, 1, 0); // default

	// display on/off control
	check_if_busy();
	display_on_off_control(DISPLAY_ON, CURSOR_OFF, BLINK_OFF); // default??

	// entry mode set
	check_if_busy();
	entry_mode_set(1, 0); // default

	check_if_busy();
}

/**************************Finish****************************/

void send_command(uint8_t command)
{
	send(command, COMMAND_MODE);
}

/**************************Finish****************************/

void send_data(uint8_t data)
{
	send(data, DATA_MODE);
}

/**************************Finish****************************/

void send(uint8_t value, uint8_t mode)
{
	DB_mode(HIGH_Z, 0);

	check_if_busy();

	if(mode == DATA_MODE)
	{
		DB_mode(NO_DB_HIGH_Z, value);

		set_RS(1);
		set_R_W(0);
	}
	else if(mode == COMMAND_MODE)
	{
		DB_mode(NO_DB_HIGH_Z, value);

		set_RS(0);
		set_R_W(0);
	}
	set_enable(0);
	delay();
	set_enable(1);
//	check_if_busy(); // should I check busy status????
	DB_mode(HIGH_Z, 0);
}

/**************************Finish****************************/

void set_RS(uint8_t mode)
{
	uint32_t gpio_outputs = MSS_GPIO_get_outputs();

	uint8_t RS_status = (gpio_outputs >> 8) & 1;

	if(RS_status != mode)
		MSS_GPIO_set_outputs(MSS_GPIO_get_outputs()^ MSS_GPIO_8_MASK);
}

/**************************Finish****************************/

void set_R_W(uint8_t mode)
{
	uint32_t gpio_outputs = MSS_GPIO_get_outputs();

	uint8_t RW_status = (gpio_outputs >> 9) & 1;

	if(RW_status != mode)
		MSS_GPIO_set_outputs(MSS_GPIO_get_outputs()^ MSS_GPIO_9_MASK);
}

/**************************Finish****************************/

void delay()
{
	int x = 0;
	for(; x < 100; x++) {}
}

/**************************Finish****************************/

void check_if_busy()
{
	set_RS(0);
	set_R_W(1);

	// wait for LCD to be not busy
	// does get_inputs work?
	uint8_t BF = (MSS_GPIO_get_inputs() >> 7) & 1;
	while(BF)
	{
		delay();
		BF = (MSS_GPIO_get_inputs() >> 7) & 1;
	}
}

/**************************Finish****************************/

void set_enable(uint8_t EN)
{
	uint32_t gpio_outputs = MSS_GPIO_get_outputs();

	uint8_t EN_status = (gpio_outputs >> 10) & 1;

	if(EN_status != EN)
		MSS_GPIO_set_outputs(MSS_GPIO_get_outputs()^ MSS_GPIO_10_MASK);
}

