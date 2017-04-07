#include <stdio.h>
#include <inttypes.h>
//#include "drivers/mss_uart/mss_uart.h"
//#include "xbee_uart.h"
#include "communication.h"
#include "controller.h"
#include "LCD.h"



int main()
{
	*LED_ADDR = 0x00;
	init_GPIO();
	uart_init();
	init_hardware_timer();
	controller_init();

	MSS_I2C_init( &g_mss_i2c1, SLAVE_SER_ADDR, MSS_I2C_PCLK_DIV_60 );
	MSS_I2C_enable_slave( &g_mss_i2c1 );
	MSS_I2C_set_slave_tx_buffer( &g_mss_i2c1, I2C_tx_buff_HP_STATUS,
			 sizeof(I2C_tx_buff_HP_STATUS) );
	MSS_I2C_set_slave_rx_buffer( &g_mss_i2c1, I2C_rx_buff_restart,
			sizeof(I2C_rx_buff_restart) );
	LCD_init();
	MSS_UART_enable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
	MSS_GPIO_config(MSS_GPIO_13, MSS_GPIO_INPUT_MODE | MSS_GPIO_IRQ_EDGE_POSITIVE);
	MSS_GPIO_enable_irq( MSS_GPIO_13 ); // turn on GPIO13 (hurt) interrupt
	NVIC_EnableIRQ(Fabric_IRQn);
	*LED_ADDR = 0xFF;



	while (1) {
		my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8) + (uint16_t)I2C_tx_buff_HP_STATUS[1];

		displayHP(my_hp);
		displayReloadLight(shootCountDown);
		displayReloadHeavy(criticalCountDown*5/CRITICAL_RELOAD_TIME);
		displaySpeeding(speeding);
		displayFreezing(stuned);
		displayHaveStun(havestun);
	}
}


