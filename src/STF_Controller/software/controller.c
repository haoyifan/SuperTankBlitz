#include "controller.h"
#include "communication.h"


uint32_t ctrl_data;
//
uint16_t servo;
uint8_t seq_ctrl;
uint16_t my_hp;
uint8_t status;
uint8_t I2C_tx_buff_HP_STATUS[3];
uint8_t I2C_rx_buff_restart[2];
uint8_t bomb_set;
uint8_t stuned = 0;
uint8_t havestun = 0;
uint8_t speeding = 0;
uint8_t canpick = 1;

uint8_t shootRDY = 1;
uint8_t shootCountDown = RELOAD_TIME;
uint8_t pickRDY = 1;
uint8_t criticalRDY = 1;
uint8_t criticalCountDown = CRITICAL_RELOAD_TIME;

float stick_ratio_f;
float stick_ratio_b;

uint8_t SLAVE_ADDR;
uint8_t MASTER_ADDR;


void reload();
void Pick_Refresh();
__attribute__ ((interrupt)) void Fabric_IRQHandler( void )
{
	//static uint8_t c = 0;
	static uint8_t shoot = 0, shoot_buff = 0;
	//c++;
	//if(c & 0x01)
	//	return;
    //printf("received \n\r");
	NVIC_ClearPendingIRQ( Fabric_IRQn );
	uint8_t cmd[7];
/////////////////////////////////////////////////////////////////////////////////////
	//check the data written by the main console
	//if game end, stop moving, wait for the console to give a commmand to start
	//if restart, then clear all the status, resume moving
/////////////////////////////////////////////////////////////////////////////////////
	ctrl_data = *(uint32_t *)(CONTROLLER_ADDR);
	if((ctrl_data >> 24) & 0x10)
	{
		Slave_init(SLAVE_ADDR);
		return;
	}
	// read controller
	my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8) + (uint16_t)I2C_tx_buff_HP_STATUS[1];

	if(my_hp == 0)
	{
		cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_DEAD;
		sendConsoleCmd(cmd, 6, SLAVE_ADDR);
		return;
	}
	if(stuned == 1){
		cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_CTRL;
		cmd[CTRL_LEFT_MOTOR_BYTE_OFFSET] = 0;
		cmd[CTRL_RIGHT_MOTOR_BYTE_OFFSET] = 0;
		cmd[CTRL_SERVO_BYTE_OFFSET] = servo >> 8;
		cmd[CTRL_SHOOT_BYTE_OFFSET] = 0x00;
		cmd[CTRL_SEQ_BYTE_OFFSET] = seq_ctrl++;
		sendConsoleCmd(cmd, 6, SLAVE_ADDR);
	}
	else{


		// start is press, send communication init pack

		// calculate moter PWM,
		int8_t joy_y = ctrl_data & 0xFF;
		int8_t joy_x = ctrl_data >> 8 & 0xFF;
		//printf("=====================\r\n");
		printf("receive joy_x: %d\r\n", joy_x);
		printf("receive joy_y: %d\r\n", joy_y);
		int32_t right_motor, left_motor;
		if (joy_y >= 0)
		{
			right_motor = (joy_y - joy_x)*stick_ratio_f;
			left_motor = (joy_y + joy_x)*stick_ratio_f;
		}
		else
		{
			right_motor = (joy_y + joy_x)*stick_ratio_b;
			left_motor = (joy_y - joy_x)*stick_ratio_b;
		}
		if(speeding){
			if(right_motor > 127)
				right_motor = 127;
			if(right_motor < -128)
				right_motor = -128;
			if(left_motor > 127)
				left_motor = 127;
			if(left_motor < -128)
				left_motor = -128;
		}
		else{
			if(right_motor > 100)
				right_motor = 100;
			if(right_motor < -100)
				right_motor = -100;
			if(left_motor > 100)
				left_motor = 100;
			if(left_motor < -100)
				left_motor = -100;
		}
		uint8_t left = (ctrl_data >> SERVO_LEFT_OFFSET) & 1;
		uint8_t right = (ctrl_data >> SERVO_RIGHT_OFFSET) & 1;
		if(!left && right)
		{
			if(servo < 0xFFFF - SERVO_STEP)
			{
				servo += SERVO_STEP;
			}
		}
		else if(left && !right)
		{
			if(servo > SERVO_STEP)
			{
				servo -= SERVO_STEP;
			}
		}
		//printf("left: %d\r\n", left_motor);
		//printf("right: %d\r\n", right_motor);
		cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_CTRL;
		cmd[CTRL_LEFT_MOTOR_BYTE_OFFSET] = left_motor;
		cmd[CTRL_RIGHT_MOTOR_BYTE_OFFSET] = right_motor;
		cmd[CTRL_SERVO_BYTE_OFFSET] = servo >> 8;
		//shoot
		shoot = (ctrl_data >> SHOOT_OFFSET) & 0x0F;
		cmd[CTRL_SHOOT_BYTE_OFFSET] = 0x00;


		uint8_t effect;
		if(ctrl_data & (1 << SHOOT_LIGHT_OFFSET) && shoot && !shoot_buff && shootRDY){
			*LED_ADDR &= ~0x08;
			shootCountDown = 0;
			startTimerOneshot(&reload, &reload_in_sec, RELOAD_TIME);
			shootRDY = 0;
			effect = NORMAL_DAMAGE;
			cmd[CTRL_SHOOT_BYTE_OFFSET] |= (0x80 | (effect<<4));
		}
		else if(ctrl_data & (1 << SHOOT_CRITICAL_OFFSET) && criticalRDY){
			*LED_ADDR &= ~0x04;
			criticalRDY = 0;
			I2C_tx_buff_HP_STATUS[2] &= ~(1 << CRITICAL_READY_STAUS_OFFSET);
			effect = CRITICAL_DAMAGE;
			criticalCountDown = 0;
			startTimerOneshot(&critical_reload, &critical_reload_in_sec, CRITICAL_RELOAD_TIME);
			cmd[CTRL_SHOOT_BYTE_OFFSET] |= (0x80 | (effect<<4));
		}
		else if(ctrl_data & (1 << SHOOT_STUN_OFFSET) && havestun){
			*LED_ADDR &= ~0x80;
			effect = STUN;
			havestun = 0;
			I2C_tx_buff_HP_STATUS[2] &= ~(1 << HAVE_STUN_STATUS_OFFSET);
			cmd[CTRL_SHOOT_BYTE_OFFSET] |= (0x80 | (effect<<4));
		}
		else{
		}



		uint8_t pick = (ctrl_data >> PICK_PACK_OFFSET) & 1;
		if (pick)
		{
			if(pickRDY && canpick)
			{
				pickRDY = 0;
				//*LED_ADDR &= ~0x08;
				startTimerOneshot(&Pick_Refresh, NULL, PICK_CLODDOWN);
				cmd[CTRL_SHOOT_BYTE_OFFSET] |= 0x40;
			}
		}

		cmd[CTRL_SEQ_BYTE_OFFSET] = seq_ctrl++;
		sendConsoleCmd(cmd, 6, SLAVE_ADDR);

		//send_data(ctrl_data, 0x04);
	}
    NVIC_ClearPendingIRQ( Fabric_IRQn );
}

uint32_t Report_Process(uint8_t* cmd){
	*LED_ADDR ^= 0xFF;
	return 0;
}

void controller_init()
{
	if(!CONTROLLER_NUM)
	{
		stick_ratio_f = 1.37;
		stick_ratio_b = 1.35;
		SLAVE_ADDR = 0x04;
		MASTER_ADDR = 0x06;
	}
	else
	{
		stick_ratio_f = 1.37;
		stick_ratio_b = 1.35;
		SLAVE_ADDR = 0x07;
		MASTER_ADDR = 0x08;
	}
	Slave_init(SLAVE_ADDR);
	seq_ctrl = 0;
	servo = 127 << 8;
	my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8)+ (uint16_t)I2C_tx_buff_HP_STATUS[1];
	my_hp= FULL_HP;
	I2C_tx_buff_HP_STATUS[0] = (my_hp >> 8) & 0xFF;
	I2C_tx_buff_HP_STATUS[1] = my_hp & 0xFF;
	bomb_set = 0;
	//start timer
	init_hardware_timer();
	startTimerOneshot(&timeup, NULL, GAME_TIME);
}

void Slave_init(uint8_t slave_addr)
{
	uint8_t cmd[6];
	cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_INIT;
	cmd[CTRL_SEQ_INIT_BYTE_OFFSET] = 0;
	seq_ctrl = 0;
	cmd[CTRL_MASTER_ADDR_BYTE_OFFSET] = MASTER_ADDR;
	if(!CONTROLLER_NUM)
	{
		cmd[CTRL_SELF_ADDR_BYTE_OFFSET] = 0x01;
		cmd[CTRL_ENEMY_ADDR_BYTE_OFFSET] = 0x00;
	}
	else
	{
		cmd[CTRL_SELF_ADDR_BYTE_OFFSET] = 0x00;
		cmd[CTRL_ENEMY_ADDR_BYTE_OFFSET] = 0x01;
	}
	sendConsoleCmd(cmd, 6, slave_addr);
}

void handle_cmd(uint8_t *cmd)
{
	static uint8_t seq_buf = 7;
	/*
	if (cmd[CMD_TYPE_BYTE_OFFSET] == CMD_REPORT_TYPE)
	{
		*LED_ADDR ^= 0x01;
		uint16_t damage = ((uint16_t)cmd[CMD_DAMGE_HIGH_BYTE_OFFSET] << 8) + (uint16_t) cmd[CMD_DAMGE_LOW_BYTE_OFFSET];
		my_hp -= damage;
		if (my_hp <= 0)
		{
			dead();
		}
//		switch (cmd[CMD_EFFECT_BYTE_OFFSET])
//		{
//		case EFFECT_SPEED_UP:
//			//set up speeding timer
//			startTimerOneshot(&speeding_timeout, NULL, SPEED_UP_TIME);
//			break;
//		case EFFECT_HEALING:
//			my_hp += HEAL_HP;
//			break;
//		case EFFECT_BOMB:
//			if (bomb_set)
//			{
//				my_hp -= BOMB_HP;
//			}
//			else
//			{
//				bomb_set = 1;
//			}
//			break;
//		default:
//			break;
//			//nothing happens, yeah
//		}
	}
	*/
	uint8_t c = *cmd;
	uint8_t type = (c >> CMD_TYPE_BIT_OFFSET) & 0x01;
	uint8_t shoot_effect = (c >> CMD_SH_EFFECT_BIT_OFFSET) & 0x03;
	uint8_t pick_effect = (c >> CMD_PICK_EFFECT_BIT_OFFSET) & 0x03;
	uint8_t seq = (c >> CMD_SEQ_BIT_OFFSET) & 0x07;
	if(seq == seq_buf)
		return;
	seq_buf = seq;
	if(type == CMD_SHOT_TYPE)
	{
		switch(shoot_effect)
		{
		case NORMAL_DAMAGE:
		{
			*LED_ADDR ^= 0x40;
			uint32_t rand = *RAND_ADDR;
			rand = (rand & 0xFF)*100/255;
			uint16_t damage = DAMAGE - 50 + rand;
			my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8) + (uint16_t)I2C_tx_buff_HP_STATUS[1];
			if(my_hp <= damage){
				my_hp = 0;

			}
			else{
				my_hp -= damage;
			}
			I2C_tx_buff_HP_STATUS[0] = (my_hp >> 8) & 0xFF;
			I2C_tx_buff_HP_STATUS[1] = my_hp & 0xFF;
			break;
		}
		case CRITICAL_DAMAGE:
			*LED_ADDR ^= 0x20;
			uint32_t rand = *RAND_ADDR;
			rand = (rand & 0xFF)*100/255;
			uint16_t damage = 2 * (DAMAGE - 50 + rand);
			my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8) + (uint16_t)I2C_tx_buff_HP_STATUS[1];
			if(my_hp <= damage){
				my_hp = 0;

			}
			else{
				my_hp -= damage;
			}
			I2C_tx_buff_HP_STATUS[0] = (my_hp >> 8) & 0xFF;
			I2C_tx_buff_HP_STATUS[1] = my_hp & 0xFF;
			break;
		case STUN:
			*LED_ADDR ^= 0x10;
			I2C_tx_buff_HP_STATUS[2] |= 1 << STUN_STATUS_OFFSET;
			stuned = 1;
			startTimerOneshot(&STUN_Refresh, NULL, STUN_TIME);
			break;
		default:
			break;
		}
		if (my_hp <= 0)
		{
			//dead()

		}
	}
	else if (type == CMD_PICK_TYPE)
	{
		canpick = 0;
		I2C_tx_buff_HP_STATUS[2] &= ~(1 << PICKUP_STATUS_OFFSET);
		startTimerOneshot(&pick_timeout, NULL, PICK_UP_CLODDOWN);
		*LED_ADDR ^= 0x01;
		switch (pick_effect)
		{
		case EFFECT_SPEED_UP:
			//set up speeding timer
			I2C_tx_buff_HP_STATUS[2] |= (1 << SPEEDUP_STATUS_OFFSET);
			speeding  = 1;
			startTimerOneshot(&speeding_timeout, NULL, SPEED_UP_TIME);
			break;
		case EFFECT_HEALING:
			my_hp = ((uint16_t)I2C_tx_buff_HP_STATUS[0] << 8) + (uint16_t)I2C_tx_buff_HP_STATUS[1];
			my_hp += HEAL_HP;
			if(my_hp >= FULL_HP)
				my_hp = FULL_HP;
			I2C_tx_buff_HP_STATUS[0] = (my_hp >> 8) & 0xFF;
			I2C_tx_buff_HP_STATUS[1] = my_hp & 0xFF;
			break;
		case EFFECT_STUN:
			havestun = 1;
			I2C_tx_buff_HP_STATUS[2] |= 1 << HAVE_STUN_STATUS_OFFSET;
			break;
		default:
			break;
			//nothing happens, yeah
		}
	}

}

void dead()
{

}

void timeup()
{

}

void speeding_timeout()
{
//	uint8_t cmd[7];
//	cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_EFFECT;
//	//cmd[CTRL_PICK_BYTE_OFFSET] = 0x10;
//	cmd[CTRL_SEQ_BYTE_OFFSET] = seq_ctrl++;
//	sendConsoleCmd(cmd, 7, SLAVE_ADDR);
	speeding  = 0;
	I2C_tx_buff_HP_STATUS[2] &= ~(1 << SPEEDUP_STATUS_OFFSET );
}

void pick_timeout()
{
//	uint8_t cmd[7];
//	cmd[CTRL_TYPE_BYTE_OFFSET] = CTRL_TYPE_EFFECT;
//	//cmd[CTRL_PICK_BYTE_OFFSET] = 0x10;
//	cmd[CTRL_SEQ_BYTE_OFFSET] = seq_ctrl++;
//	sendConsoleCmd(cmd, 7, SLAVE_ADDR);
	canpick = 1;
	I2C_tx_buff_HP_STATUS[2] |= (1 << PICKUP_STATUS_OFFSET );
}

void reload()
{
	*LED_ADDR |= 0x08;
	shootRDY = 1;
	shootCountDown = RELOAD_TIME;
}

void reload_in_sec()
{
	shootCountDown += 1;
}

void Pick_Refresh()
{
	*LED_ADDR |= 0x40;
	pickRDY = 1;
}

void STUN_Refresh()
{
	//*LED_ADDR |= 0x40;
	stuned = 0;
	I2C_tx_buff_HP_STATUS[2] &= ~(1 << STUN_STATUS_OFFSET);
}

void critical_reload()
{
	*LED_ADDR |= 0x04;
	criticalRDY = 1;
	criticalCountDown = CRITICAL_RELOAD_TIME;
	I2C_tx_buff_HP_STATUS[2] |= (1 << CRITICAL_READY_STAUS_OFFSET);
}

void critical_reload_in_sec()
{
	criticalCountDown += 1;
}

void game_init()
{
	my_hp= FULL_HP;
	I2C_tx_buff_HP_STATUS[0] = (my_hp >> 8) & 0xFF;
	I2C_tx_buff_HP_STATUS[1] = my_hp & 0xFF;
	I2C_tx_buff_HP_STATUS[2] = 0;
	I2C_tx_buff_HP_STATUS[2] |= 1 << PICKUP_STATUS_OFFSET;
	I2C_tx_buff_HP_STATUS[2] |= 1 << CRITICAL_READY_STAUS_OFFSET;

	stuned = 0;
	havestun = 0;
	speeding = 0;
	canpick = 1;

	//shootRDY = 1;
	//shootCountDown = RELOAD_TIME;
	//pickRDY = 1;
	//criticalRDY = 1;
	//criticalCountDown = CRITICAL_RELOAD_TIME;
}

__attribute__ ((interrupt)) void GPIO13_IRQHandler()
{
	game_init();
	MSS_GPIO_clear_irq( MSS_GPIO_13 );
}
