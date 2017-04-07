#include "control.h"
#include "rfid.h"
#include "disp_func.h"

uint32_t pulseWidth;
uint8_t reportSeq;
uint8_t seq_ctrl;
uint8_t connection;

uint8_t ENEMY_ADDR = 0x01;
uint8_t SELF_ADDR = 0x00;
uint8_t MASTER_ADDR;

uint8_t speeding;

uint8_t disp_num = MY_DEBUG;
uint16_t damage_disp = 0;
uint16_t shooting_effect_disp = 0;
uint16_t rec_effect_disp = 0;
uint8_t pick_effect_disp = 0;
uint8_t left_disp = 0;
uint8_t right_disp = 0;

uint8_t dead_sound = 0;

uint32_t Tank_Act(uint8_t* cmd)
{
	if(cmd[CTRL_TYPE_BYTE_OFFSET] == CTRL_TYPE_CTRL)
	{
		dead_sound = 0;
		if(!connection);
		//return 1;
		int8_t left_motor = cmd[CTRL_LEFT_MOTOR_BYTE_OFFSET];
		int8_t right_motor = cmd[CTRL_RIGHT_MOTOR_BYTE_OFFSET];
		pulseWidth = ((uint32_t)cmd[CTRL_SERVO_BYTE_OFFSET]*(UPPER_BOUND - LOWER_BOUND))/255 + LOWER_BOUND;
		uint8_t shoot = cmd[CTRL_SHOOT_BYTE_OFFSET] & 0x80;
		uint8_t pick_pack = cmd[CTRL_SHOOT_BYTE_OFFSET] & 0x40;
		uint8_t shooting_effect = (cmd[CTRL_SHOOT_BYTE_OFFSET] & 0x30) >> 4;
		uint8_t seq_recv = cmd[CTRL_SEQ_BYTE_OFFSET];
		if(seq_ctrl != seq_recv)
		{
			*LED_ADDR = 0;
			//printf("=====\n\r");
			//printf("recived seq: %d\r\n", seq_recv);
			//printf("expected seq: %d\r\n", seq_ctrl);
			connection = 0;
			//return 0;
		}
		seq_ctrl = seq_recv + 1;
		uint32_t gpio_outputs;
		gpio_outputs = MSS_GPIO_get_outputs();
		gpio_outputs &= 0xFFFFFFC3;
		// left motor
		/*if(left_motor > 0)
		{
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_FORWARD);
		}
		else if(left_motor < 0)
		{
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_BACKWARD);
			left_motor = -left_motor;
		}
		else
		{
			MSS_GPIO_set_outputs(gpio_outputs & ~LEFT_STOP);
		}
		// right motor
		if(right_motor < 0)
		{
			MSS_GPIO_set_outputs(gpio_outputs | RIGHT_FORWARD);
		}
		else if(right_motor <0)
		{
			MSS_GPIO_set_outputs(gpio_outputs | RIGHT_BACKWARD);
			right_motor = -right_motor;
		}
		else
		{
			MSS_GPIO_set_outputs(gpio_outputs & ~RIGHT_STOP);
		}*/

		if(left_motor >= 0 && right_motor >= 0)
		{
			//*LED_ADDR = ~0xA0;
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_FORWARD | RIGHT_FORWARD);
		}
		else if (left_motor >= 0 && right_motor < 0)
		{
			//*LED_ADDR &= ~0x90;
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_FORWARD | RIGHT_BACKWARD);
			right_motor = -right_motor; // what are you doing here!!!! Yifan
		}
		else if(left_motor < 0 && right_motor >= 0)
		{
			//*LED_ADDR &= ~0x60;
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_BACKWARD | RIGHT_FORWARD);
			left_motor = -left_motor;
		}
		else
		{
			//*LED_ADDR &= ~0x50;
			MSS_GPIO_set_outputs(gpio_outputs | LEFT_BACKWARD | RIGHT_BACKWARD);
			right_motor = -right_motor;
			left_motor = -left_motor;
		}

		if(right_motor > 127)
			right_motor = 127;
		if(left_motor > 127)
			left_motor = 127;
		//printf("=====================\r\n");
		//printf("left: %d\r\n", left_motor);
		//printf("right: %d\r\n", right_motor);

		left_disp = left_motor;
		right_disp = right_motor;
		disp_func_array[3] = DispSpeed;
		*LEFT_ADDR = (uint32_t)(left_motor * 15680);
		*RIGHT_ADDR = (uint32_t)(right_motor * 15680);


		// servo. This if statement check if the servo reaches the boundary
		if(pulseWidth < *SERVO_ADDR)
			*LED_ADDR &= ~0x08;
		else
			*LED_ADDR |= 0x08;
		if(pulseWidth > *SERVO_ADDR)
			*LED_ADDR &= ~0x04;
		else
			*LED_ADDR |= 0x04;
		*SERVO_ADDR = pulseWidth;

		// shooting
		if(shoot)
		{
			if (shooting_effect == NORMAL_DAMAGE)
				*AUDIO_ADDR = SHOOT_SOUND;
			else if (shooting_effect == CRITICAL_DAMAGE)
				*AUDIO_ADDR = CRITICAL_SHOOT_SOUND;
			*LED_ADDR ^= 0x02;
			*SHOOT_ADDR = ENEMY_ADDR | shooting_effect | 0x08;
			shooting_effect_disp = shooting_effect;
			disp_num = MY_DEBUG;
			disp_func_array[1] = DispShootEffect;
		}

		//pick package
		if (pick_pack)
		{
			*LED_ADDR ^= 0x80;
			//uint8_t rfid = 0;
			uint8_t rfid = reachRFID();
			//printf("The tag id is: %d\r\n", rfid);
			uint8_t effect = 0;
			switch (rfid){
			case SPEED_UP1:
			case SPEED_UP2:
				//printf("speed up!\r\n");
				effect = EFFECT_SPEED_UP;
				*AUDIO_ADDR = PICK1_SOUND;
				break;
			case HEALING1:
			case HEALING2:
				//printf("healed!\r\n");
				effect = EFFECT_HEALING;
				*AUDIO_ADDR = HEALING_SOUND;
				break;
			case STUN1:
			case STUN2:
				//printf("get stun effect!\r\n");
				effect = EFFECT_STUN;
				*AUDIO_ADDR = PICK2_SOUND;
				break;
			case 0:
				return 0;
			default:
				//printf("fail to pick package\r\n");
				//*LED_ADDR |= 0x80;
				break;
			}

			uint8_t cmd = 0;
			cmd |= (CMD_PICK_TYPE << CMD_TYPE_BIT_OFFSET);
			cmd |= (effect << CMD_PICK_EFFECT_BIT_OFFSET);
			cmd |= ((reportSeq++ & 0x07) << CMD_SEQ_BIT_OFFSET);
			sendConsoleCmd(&cmd, 1, MASTER_ADDR);
			pick_effect_disp = effect;
			disp_func_array[0] = DispPickEffect;
			/*
			uint8_t cmd[5];
			cmd[CMD_TYPE_BYTE_OFFSET] = CMD_REPORT_TYPE;
			cmd[CMD_DAMGE_HIGH_BYTE_OFFSET] = 0;
			cmd[CMD_DAMGE_LOW_BYTE_OFFSET] = 0;
			cmd[CMD_EFFECT_BYTE_OFFSET] = effect;
			cmd[CMD_SEQ_BYTE_OFFSET] = reportSeq;
			reportSeq++;
			sendConsoleCmd(cmd, 5, MASTER_ADDR);
			 */
		}
	}
	else if(cmd[CTRL_TYPE_BYTE_OFFSET] == CTRL_TYPE_INIT)
	{
		seq_ctrl = cmd[CTRL_SEQ_INIT_BYTE_OFFSET];
		MASTER_ADDR =cmd[CTRL_MASTER_ADDR_BYTE_OFFSET];
		SELF_ADDR = cmd[CTRL_SELF_ADDR_BYTE_OFFSET];
		ENEMY_ADDR = cmd[CTRL_ENEMY_ADDR_BYTE_OFFSET];
		ENEMY_ADDR = ENEMY_ADDR << SHOOTING_ADDR_BIT_OFFSET;
		*HARM_ADDR = SELF_ADDR;
		connection = 1;
		*LED_ADDR = 0xFF;
		disp_num = CRY;
	}
	else if (cmd[CTRL_TYPE_BYTE_OFFSET] == CTRL_TYPE_DEAD)
	{
		if(dead_sound == 0)
		{
			*AUDIO_ADDR = DEAD_SOUND;
		}
		dead_sound = 1;
		*LEFT_ADDR = 0;
		*RIGHT_ADDR = 0;
		//if (cmd[CTRL_PICK_BYTE_OFFSET] == 0x10 )
		//{
		//speeding time out
		//speeding = 0;
		//}
	}
	return 0;
}


__attribute__ ((interrupt)) void GPIO1_IRQHandler()
{
	/*
	// hit
	// read *HARM_ADDR to get detail, current 2 bytes for different effect.
	uint8_t cmd[5];
	// generate foating damage. considering move this part to controller
	uint32_t rand = *RAND_ADDR;
	*AUDIO_ADDR = HARM_SOUND;
	rand = (rand & 0xFF)*100/255;
	uint16_t damage = DAMAGE - 50 + rand;
	uint8_t rec_effect = (*HARM_ADDR) & 0x03;
	//printf("damage received: %d\n\r", damage);
	// send damage reprot
	cmd[CMD_TYPE_BYTE_OFFSET] = CMD_REPORT_TYPE;
	cmd[CMD_DAMGE_HIGH_BYTE_OFFSET] = (uint8_t)((damage >> 8) & 0xFF);
	cmd[CMD_DAMGE_LOW_BYTE_OFFSET] = (uint8_t)(damage & 0xFF);
	cmd[CMD_EFFECT_BYTE_OFFSET] = rec_effect;
	cmd[CMD_SEQ_BYTE_OFFSET] = reportSeq;
	reportSeq++;
	sendConsoleCmd(cmd, 5, MASTER_ADDR);
	//printU16(0,0,damage);
	disp_num = MY_DEBUG;
	damage_disp = ((uint16_t)cmd[CMD_DAMGE_HIGH_BYTE_OFFSET] << 8) + (uint16_t) cmd[CMD_DAMGE_LOW_BYTE_OFFSET];
	disp_func_array[0] = DispDamage;
	rec_effect_disp = rec_effect;
//	rec_effect_disp = 4;
	disp_func_array[2] = DispRecEffect;
	*/
	uint8_t cmd = 0;

	uint8_t rec_effect = (*HARM_ADDR) & 0x03;
	if (rec_effect == NORMAL_DAMAGE || rec_effect == CRITICAL_DAMAGE)
		*AUDIO_ADDR = SHOT_SOUND;
	else if (rec_effect == STUN)
		*AUDIO_ADDR = STUNNED_SOUND;
	cmd |= (CMD_SHOT_TYPE << CMD_SH_EFFECT_BIT_OFFSET);
	cmd |= (rec_effect << CMD_SH_EFFECT_BIT_OFFSET);
	cmd |= ((reportSeq++ & 0x07) << CMD_SEQ_BIT_OFFSET);
	sendConsoleCmd(&cmd, 1, MASTER_ADDR);
	disp_num = MY_DEBUG;
	rec_effect_disp = rec_effect;
	disp_func_array[2] = DispRecEffect;
	*LED_ADDR ^= 0x01;
	MSS_GPIO_clear_irq( MSS_GPIO_1 );
}

void control_init()
{
	//MSS_GPIO_init();
	//MSS_GPIO_config(MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE); // RFID reset
	MSS_GPIO_config(MSS_GPIO_2, MSS_GPIO_OUTPUT_MODE); // tank En1
	MSS_GPIO_config(MSS_GPIO_3, MSS_GPIO_OUTPUT_MODE); // tank En2
	MSS_GPIO_config(MSS_GPIO_4, MSS_GPIO_OUTPUT_MODE); // tank En3
	MSS_GPIO_config(MSS_GPIO_5, MSS_GPIO_OUTPUT_MODE); // tank En4
	MSS_GPIO_config(MSS_GPIO_6, MSS_GPIO_OUTPUT_MODE); // Xbee reset
	//MSS_GPIO_set_output(MSS_GPIO_2, 0);
	//MSS_GPIO_set_outputs(0x0010);
	pulseWidth = 150000;
	reportSeq = 0;
	connection = 0;
	speeding = 0;
}

