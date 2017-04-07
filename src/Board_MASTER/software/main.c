#include <stdio.h>
#include <inttypes.h>
#include "drivers/mss_spi/drivers/mss_spi/mss_spi.h"
#include "drivers/mss_i2c/mss_i2c.h"
#include "drivers/mss_uart/mss_uart.h"
#include "drivers/mss_gpio/mss_gpio.h"
// uint32_t g_slave_rx_frame = 0;
//
// void slave_frame_handler( uint32_t rx_frame )
//{
//	 g_slave_rx_frame = rx_frame;
//	 printf("%d\n\t", g_slave_rx_frame);
//}
//int setup_slave( void )
//{
//	const uint16_t frame_size = 32;
//	MSS_SPI_init( &g_mss_spi1 );
//	MSS_SPI_configure_slave_mode
//	(
//			&g_mss_spi1,
//			MSS_SPI_MODE1,
//			MSS_SPI_PCLK_DIV_256,
//			frame_size
//	);
//	MSS_SPI_set_frame_rx_handler( &g_mss_spi1, slave_frame_handler );
//	return 0;
//}
//
//int main()
//{
//	uint32_t slave_tx_frame = 1231231;
//	setup_slave();
//    MSS_SPI_set_slave_tx_frame( &g_mss_spi1, slave_tx_frame );
//    while(1){
//    }
//	return(0);
//}

/*
 * WHAT CONSOLE NEEDS TO SEND TO GUI:
 * HP uint16_t
 * Speed status bit or uint8_t
 * Stun status bit or uint8_t
 * if hit(light or hard)
 * Stun item bit or uint8_t
 * pick up health
 */

#define I2C_DUMMY_ADDR   0x10u
//#define DATA_LENGTH      10u
#define Console_To_Controller_DATA_LENGTH 1
#define Controller_To_Console_DATA_LENGTH 3
#define HP_OFFSET 0
#define PERKSTATUS_OFFSET 1


uint8_t  rx_buffer_p1[Controller_To_Console_DATA_LENGTH] = {0, 0, 1};
uint8_t  rx_buffer_p2[Controller_To_Console_DATA_LENGTH] = {0, 0, 1};
uint8_t  tx_buffer[2] = {1 , 1};
//uint8_t  read_length = DATA_LENGTH ;
uint8_t	message[] = "Hello World";
__attribute__ ((interrupt)) void Fabric_IRQHandler( void )

{
	uint8_t  target_slave_addr_p1 = 0x12;
	uint8_t  target_slave_addr_p2 = 0x20;
	MSS_GPIO_set_output(MSS_GPIO_0, 1);
	MSS_GPIO_set_output(MSS_GPIO_1, 1);
	asm("nop;");
	MSS_GPIO_set_output(MSS_GPIO_0, 0);
	MSS_GPIO_set_output(MSS_GPIO_1, 0);
}

int main( void )
{
	uint8_t  target_slave_addr_p1 = 0x12;
	uint8_t  target_slave_addr_p2 = 0x20;
	uint16_t hp1 = 0;
	uint16_t hp2 = 0;

	// this value indicates when the console will read the data from both consoles
	// when it is multiple of 1000, it reads. Otherwise, it skips
	uint32_t read_count = 0;
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE );
	MSS_GPIO_config( MSS_GPIO_1, MSS_GPIO_OUTPUT_MODE );
	mss_i2c_status_t status = MSS_I2C_IN_PROGRESS;

	// Initialize MSS I2C peripheral
	MSS_I2C_init( &g_mss_i2c1, I2C_DUMMY_ADDR, MSS_I2C_PCLK_DIV_60);

	// initialize UART0. This UART port is connected to GUI
	MSS_UART_init( &g_mss_uart0, MSS_UART_9600_BAUD,
			MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT );
	NVIC_EnableIRQ(Fabric_IRQn);
	while(1){
		if(++read_count % 1000000 == 0) {
			// Read data from target slave using MSS I2C 1.
			MSS_I2C_read( &g_mss_i2c1, target_slave_addr_p1, rx_buffer_p1, Controller_To_Console_DATA_LENGTH,
					MSS_I2C_RELEASE_BUS );
			status = MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT );
			MSS_I2C_read( &g_mss_i2c1, target_slave_addr_p2, rx_buffer_p2, Controller_To_Console_DATA_LENGTH,
					MSS_I2C_RELEASE_BUS );
			status = MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT );
			hp1 = ((uint16_t) rx_buffer_p1[0] << 8) + (uint16_t) rx_buffer_p1[1];
			hp2 = ((uint16_t) rx_buffer_p2[0] << 8) + (uint16_t) rx_buffer_p2[1];
			printf("0x12: %d\r\n", hp1);
			printf("0x20: %d\r\n", hp2);
			// if find dead, write to p1 p2 slave.
			// MSS_I2C_write( &g_mss_i2c1, target_slave_addr, rx_buffer, read_length,
			//                     MSS_I2C_RELEASE_BUS );
			// status = MSS_I2C_wait_complete( &g_mss_i2c1, MSS_I2C_NO_TIMEOUT );

			// send to gui by uart
			// MSS_UART_polled_tx( &g_mss_uart0, message, sizeof(message) );
			// wait complete?
		}

	}
}
