#include "communication.h"
#include "control.h"
#include "rfid.h"
#include "oled.h"
#include "disp_func.h"

int main()
{
	int i = 0;
	*LED_ADDR = 0x00;
	DispInit();
	AddicoreRFID_Init(); // initialize RFID. GPIO init is called here
	control_init(); // initialize control system (Motor i/o port)
	//----------------OLED START------
	oled_init(1);
	clear(ALL);
	display();
	oled_delay(100);
	clear(PAGE);
	//---------------OLED FINISH-----
	MSS_GPIO_set_output(MSS_GPIO_6, 1);
	uart_init();
	*LED_ADDR = 0xff;
	for(i = 0; i < 1000000; i++) {
		asm("nop;");
	}
	*LED_ADDR = 0x00;


	//printf("Hello world!\r\n");
	MSS_GPIO_enable_irq( MSS_GPIO_1 ); // turn on GPIO1 (hurt) interrupt
	MSS_UART_enable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ ); // turn on communication interrupt
	while(1){
		int j = 0;
		switch (disp_num)
		{
		case MY_DEBUG:
			clear(PAGE);
			for(j = 0; j < 10; ++j)
			{
				if(disp_func_array[j] != 0)
				{
					(*disp_func_array[j])();
				}
			}
			break;
		case CRY:
			drawCry();
			break;
		default:
			break;
		}

		XBee_timeout_timer--;
		//printf("Hello world!\r\n");
		if( XBee_timeout_timer < 0)
		{
			printf("reset Xbee\r\n");
			MSS_GPIO_set_output(MSS_GPIO_6, 0);
			XBee_timeout_timer = 500000;
			MSS_UART_get_rx_status(&g_mss_uart1);
			UART_CLEAR_BUFFER;
			uint32_t i = 0;
			for(i = 0; i < 10000000; i++) {
				asm("nop;");
			}
			MSS_GPIO_set_output(MSS_GPIO_6, 1);
		}
			//reset
		//MSS_UART_enable_irq( &g_mss_uart1, MSS_UART_RBF_IRQ );
	}
}
