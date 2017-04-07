#include "communication.h"


int32_t XBee_timeout_timer;

void my_rx_handler( mss_uart_instance_t * this_uart )
{
	MSS_UART_disable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
	uint8_t rx_buf;
	/*
	static uint8_t buff1[MAX_RX_DATA_SIZE];
	static uint8_t buff2[MAX_RX_DATA_SIZE];
	static uint8_t* rx_buff = buff1;
	static uint8_t* cmd = buff2;
	static uint8_t* temp_ptr;
	static uint8_t hand = 0;
	static uint8_t checkSum = 0;
	static uint8_t err_status, done = 0;
	static uint8_t state = WAITING;
	static uint16_t pack_size = 11;
	uint8_t temp;
	XBee_timeout_timer = 5000000;
	MSS_UART_disable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
	err_status = MSS_UART_get_rx_status(&g_mss_uart1);
	//printf("error: %x\r\n", err_status);

	if( 1 )
	{
		//if no error in uart
		while(MSS_UART_get_rx(&g_mss_uart1, &temp, 1))
		{
			if(state == WAITING)
			{
				if(temp == 0x7e)
				{
					state = PENDING;
				}
			}
			if(state == PENDING)
			{
				rx_buff[hand++]=temp;
				if(hand == 3)
				{
					pack_size = (rx_buff[1] << 8) + rx_buff[2];
					if(pack_size != CMD_LENGTH - 4)
					{
						state = WAITING;
						hand = 0;
						checkSum = 0;
						continue;
					}
					else{
						hand = 3;
					}
				}
				if(hand == CMD_LENGTH)
				{
					if(((checkSum - rx_buff[0] - rx_buff[1] - rx_buff[2] + temp) & 0xff) == 0xff)
					{
						temp_ptr = cmd;
						cmd = rx_buff;
						rx_buff = temp_ptr;
						done = 1;
						handle_cmd(cmd + XBEE_HEADER_LENGTH);
						//Report_Process(cmd + XBEE_HEADER_LENGTH);
					}
					else
					{
						done = 0;
					}
					state = WAITING;
					hand = 0;
					checkSum = 0;
					continue;
				}
				checkSum += temp;
			}
		}
	}
	else
	{
		UART_CLEAR_BUFFER;
	}
	if(done)
		done = 0;
	else
	{
		MSS_UART_enable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
		return;
	}
	// pass cmd buffer to control function
	//printf("valid cmd received, seq: %d\r\n", cmd[12]);
	//printf("expected seq: %d\r\n", seq);
	*/
	///////////////////////////////////////////////////
	MSS_UART_get_rx(&g_mss_uart1, &rx_buf, 1);
	handle_cmd(&rx_buf);
	///////////////////////////////////////////////////


	MSS_UART_enable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
}

void sendConsoleCmd(uint8_t* data, uint8_t length, uint8_t addr)
{
	static uint8_t tx_buff[20];
	uint8_t i = 0, sum = 0;;
	tx_buff[0] = 0x7E; // delimiter
//	tx_buff[1] = 0x00; // length
//	tx_buff[2] = 5+length; // length
//	tx_buff[3] = 0x01; // cmdID/Frame Type
//	tx_buff[4] = 0x00; //Frame ID. Should always be 0x01
//	tx_buff[5] = 0x00; // address
//	tx_buff[6] = addr; // address
//	tx_buff[7] = 0x00; // option

	for(i = 0; i < length; ++i)
	{
		tx_buff[XBEE_HEADER_LENGTH + i] = data[i];
	}
	for(i = 0; i < XBEE_HEADER_LENGTH + length; ++i)
	{
		sum +=tx_buff[i];
	}
	tx_buff[i] = 0xFF - sum;
	++i;
	MSS_UART_polled_tx( &g_mss_uart1, tx_buff, i );
	while ( ! (MSS_UART_TEMT & MSS_UART_get_tx_status( &g_mss_uart1 ) ) );
}

void uart_init()
{
	MSS_GPIO_config(MSS_GPIO_15, MSS_GPIO_OUTPUT_MODE); // Xbee reset
	MSS_GPIO_set_output(MSS_GPIO_15, 1);
	MSS_UART_init
	(
		 &g_mss_uart1,
		 MSS_UART_57600_BAUD,
		 MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT
	);
	MSS_UART_set_rx_handler( &g_mss_uart1,
						   my_rx_handler,
						   MSS_UART_FIFO_SINGLE_BYTE );
	printf("reset Xbee\r\n");
	MSS_GPIO_set_output(MSS_GPIO_15, 0);
	MSS_UART_get_rx_status(&g_mss_uart1);
	UART_CLEAR_BUFFER;
	uint32_t i = 0;
	for(i = 0; i < 10000000; i++) {
		asm("nop;");
	}
	MSS_GPIO_set_output(MSS_GPIO_15, 1);

}
