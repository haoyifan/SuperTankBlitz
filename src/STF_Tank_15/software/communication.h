#ifndef _COMMU_H_
#define _COMMU_H_

#include <inttypes.h>
#include <stdio.h>
#include "drivers/mss_uart/mss_uart.h"

#define WAITING 0x01
#define PENDING 0x02

#define MAX_RX_DATA_SIZE 0xff
#define CMD_LENGTH 15
#define XBEE_HEADER_LENGTH 1

#define UART1_FCR 0x40010008
#define UART_CLEAR_BUFFER *(volatile uint8_t*)UART1_FCR = 0b10000010

extern int32_t XBee_timeout_timer;

void my_rx_handler( mss_uart_instance_t * this_uart );
void sendConsoleCmd(uint8_t* data, uint8_t length, uint8_t addr);
void uart_init();


#endif

