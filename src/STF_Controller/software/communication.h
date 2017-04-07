#ifndef _COMMU_H_
#define _COMMU_H_
#include <inttypes.h>
#include <stdio.h>
#include "controller.h"
#include "timer.h"
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include "drivers/mss_i2c/mss_i2c.h"
/*
#define DBG(...)\
	printf("%d\r\n", __LINE__);\
	printf(__VA_ARGS__);
*/

#define MASTER_XBEE_ADDR 0x04

#define WAITING 0x01
#define PENDING 0x02

#define MAX_RX_DATA_SIZE 0xff
// CMD_LENGTH control the bytes received in every pack.
// modify this, if change protocol
#define CMD_LENGTH 14
#define XBEE_HEADER_LENGTH 1


#define UART1_FCR 0x40010008
#define UART_CLEAR_BUFFER *(volatile uint8_t*)UART1_FCR = 0b00000010

extern int32_t XBee_timeout_timer;

void my_rx_handler( mss_uart_instance_t* this_uart );
// send send data in buffer <data>, with length <length>, to addr.
// addr is set in controller_init(), determined by controller number
void sendConsoleCmd(uint8_t* data, uint8_t length, uint8_t addr);
void uart_init();


#endif

