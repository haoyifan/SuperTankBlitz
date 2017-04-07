#include <inttypes.h>
#include <stdio.h>
#include "drivers/mss_uart/mss_uart.h"




uint8_t rx_buff[14];
uint8_t data[4];
uint8_t seq;

void init_uart()
{
	MSS_UART_init
		     (
		         &g_mss_uart1,
		         MSS_UART_9600_BAUD,
		         MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
		     );
}

void send_data(uint32_t data, uint8_t dest)
{
	//data actually must be 4 byte
	uint8_t tx_buff[14];
	tx_buff[0] = 0x7E; // delimiter
	tx_buff[1] = 0x00; // length
	tx_buff[2] = 0x0A; // length
	tx_buff[3] = 0x01; // cmdID/Frame Type
	tx_buff[4] = 0x01; //Frame ID. Should always be 0x01

	tx_buff[5] = 0x00; // address
	tx_buff[6] = dest; // address
	tx_buff[7] = 0x00; // option
	tx_buff[8] = (data >> 24) & 0xFF; // data
	tx_buff[9] = (data >> 16) & 0xFF; // data
	tx_buff[10] = (data >> 8) & 0xFF; // data
	tx_buff[11] = data & 0xFF; // data
	if(seq == 100){
		seq = 0;
	}
	else{
		seq ++;
	}
	tx_buff[12] = seq; //useless byte
	//printf("my seq: %d\n\r", seq);
	uint8_t sum = 0;
	uint8_t i = 3;

	//calculate the sum
	for (; i < 13; i++)
	{
		sum +=  tx_buff[i];
	}

	// calculate the checksum
	tx_buff[13] = 0xFF - sum;

	MSS_UART_polled_tx( &g_mss_uart1, tx_buff, sizeof(tx_buff) );

	//MSS_UART_irq_tx( &g_mss_uart1, tx_buff, sizeof(tx_buff));
	while ( ! (MSS_UART_TEMT & MSS_UART_get_tx_status( &g_mss_uart1 ) ) )
	    {
	        ;
	    }
}












