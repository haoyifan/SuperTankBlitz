/*
 * LCD.c
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

void displayHP(uint16_t hp)
{
	set_DDRAM_addr(0x00);
	send_data('H');
	send_data('P');
	send_data(':');
	char hpvalue[6] = "00000";
	uint8_t i;
	uint16_t base = 10000;
	for (i = 0; i < 5; i++)
	{
		hpvalue[i] += (hp / base) % 10;
		base /= 10;
	}
	for (i = 0; i < 5; i++)
	{
		send_data(hpvalue[i]);
	}
}

void displayReloadLight(uint16_t time)
{
	set_DDRAM_addr(0x40);
	send_data('L');
	send_data('[');
	uint8_t i;
	for (i = 0; i < 5; i++)
	{
		if (time > i)
		{
			send_data(block_char);
		}
		else
		{
			send_data(' ');
		}
	}
	set_DDRAM_addr(0x47);
	send_data(']');
}

void displayReloadHeavy(uint16_t time)
{
	set_DDRAM_addr(0x48);
	send_data('H');
	send_data('[');
	uint8_t i;
	for (i = 0; i < 5; i++)
	{
		if (time > i)
		{
			send_data(block_char);
		}
		else
		{
			send_data(' ');
		}
	}
	set_DDRAM_addr(0x4f);
	send_data(']');
}

void displaySpeeding(uint8_t speeding)
{
	set_DDRAM_addr(0x09);
	if (speeding){
		send_data('S');
	}
	else{
		send_data(' ');
	}
}

void displayHaveStun(uint8_t have)
{
	set_DDRAM_addr(0x0a);
	if (have){
		send_data('H');
	}
	else{
		send_data(' ');
	}
}

void displayFreezing(uint8_t freezing)
{
	set_DDRAM_addr(0x0b);
	if (freezing){
		send_data('F');
	}
	else{
		send_data(' ');
	}
}

/**************************Finish****************************/

void clear()
{
	send_command(LCD_CLEARDISPLAY);
}

/**************************Finish****************************/

void return_home()
{
	send_command(LCD_RETURNHOME);
}

/**************************Finish****************************/

void display_on_off_control(uint8_t D, uint8_t C, uint8_t B)
{
	uint8_t command = (0x08 | D << 2 | C << 1 | B);
	send_command(command);
}

/**************************Finish****************************/

void entry_mode_set(uint8_t ID, uint8_t SH)
{
	uint8_t command = (0x04 | ID << 1 | SH);
	send_command(command);
}

/**************************Finish****************************/

void function_set(uint8_t DL, uint8_t N, uint8_t F)
{
	uint8_t command = (0x20 | DL << 4 | N << 3 | F << 2);
	send_command(command);
}

/**************************Finish****************************/

void set_CGRAM_addr(uint8_t addr)
{
	uint8_t real_addr = addr & 0x3F;
	uint8_t command = (0x40 | real_addr);
	send_command(command);
}

/**************************Finish****************************/

void set_DDRAM_addr(uint8_t addr)
{
	uint8_t real_addr = addr & 0x7F;
	uint8_t command = (0x80 | real_addr);
	send_command(command);
}

/**************************Finish****************************/

void write_data(uint8_t data)
{
	send_data(data);
}

uint8_t read_data()
{
	DB_mode(HIGH_Z, 0);

	set_RS(1);
	set_R_W(1);

	set_enable(1);
	delay();
	set_enable(0);
	// ???????????check_if_busy(); // should I check busy status????

	return MSS_GPIO_get_inputs() & 0xFF;
}

void write_string(char* string)
{
	char* it;
	for (it = string; it != '\0'; ++it)
	{
		write_data(*it);
	}
}
