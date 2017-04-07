#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include <inttypes.h>
#include "timer.h"

#define CONTROLLER_ADDR (volatile uint32_t*)0x40050000
#define LED_ADDR 		(volatile uint32_t*)0x40050100
#define RAND_ADDR (volatile uint32_t*)0x40050200

#define SERVO_STEP 700

//timing in sec
#define GAME_TIME 180
#define RELOAD_TIME 5
#define CRITICAL_RELOAD_TIME 20
#define SPEED_UP_TIME 20
#define PICK_CLODDOWN 1
#define PICK_UP_CLODDOWN 30
#define STUN_TIME 10

//hp
#define FULL_HP 2000
#define HEAL_HP 500
#define BOMB_HP 500

//controller data
#define PICK_PACK_OFFSET 30  		//B
#define SERVO_LEFT_OFFSET 21 		//L
#define SERVO_RIGHT_OFFSET 20		//R
#define SHOOT_LIGHT_OFFSET 19		//C-up
#define SHOOT_CRITICAL_OFFSET 18	//C-down
#define SHOOT_STUN_OFFSET 17		//C-Left
#define SHOOT_EXPLOSIVE_OFFSET 16	//C-Right
#define SHOOT_OFFSET 16				//C-Right

// shell effect
#define NORMAL_DAMAGE 0x00
#define CRITICAL_DAMAGE 0x01
#define STUN 0x02
#define EXPLOSIVE 0x03

//cmd_effect_byte types
#define EFFECT_SPEED_UP 0x01
#define EFFECT_HEALING 0x02
#define EFFECT_STUN 0x03

//cmd_type_byte types
#define CMD_REPORT_TYPE 0
#define CMD_RESPONSE_TYPE 1

//cmd bytes
#define CMD_TYPE_BYTE_OFFSET 0 		//report or response
#define CMD_DAMGE_LOW_BYTE_OFFSET 1
#define CMD_DAMGE_HIGH_BYTE_OFFSET 2
#define CMD_EFFECT_BYTE_OFFSET 3
#define CMD_SEQ_BYTE_OFFSET 4

#define CMD_SHOT_TYPE 0
#define CMD_PICK_TYPE 1

#define CMD_TYPE_BIT_OFFSET 7 // 1 bit width
#define CMD_SH_EFFECT_BIT_OFFSET 5 // 2 bit width
#define CMD_PICK_EFFECT_BIT_OFFSET 3 // 2 bit width
#define CMD_SEQ_BIT_OFFSET 0 // 3 bit width

//ctrl_type_ctrl bytes
#define CTRL_TYPE_BYTE_OFFSET 0
#define CTRL_LEFT_MOTOR_BYTE_OFFSET 1
#define CTRL_RIGHT_MOTOR_BYTE_OFFSET 2
#define CTRL_SERVO_BYTE_OFFSET 3
#define CTRL_SHOOT_BYTE_OFFSET 4
// shoot command only use the first bit, can add pick and other effect into this
//#define CTRL_PICK_BYTE_OFFSET 5
#define CTRL_SEQ_BYTE_OFFSET 5

//ctrl_type_init bytes
#define CTRL_SEQ_INIT_BYTE_OFFSET 1
#define CTRL_MASTER_ADDR_BYTE_OFFSET 2
#define CTRL_SELF_ADDR_BYTE_OFFSET 3
#define CTRL_ENEMY_ADDR_BYTE_OFFSET 4

#define CTRL_TYPE_CTRL 0
#define CTRL_TYPE_INIT 1
#define CTRL_TYPE_DEAD 2

#define DAMAGE 200

extern uint16_t my_hp;
extern uint8_t status;
extern uint8_t shootCountDown;
extern uint8_t criticalCountDown;
extern uint8_t speeding;
extern uint8_t stuned;
extern uint8_t havestun;
extern uint8_t I2C_tx_buff_HP_STATUS[3];
extern uint8_t I2C_rx_buff_restart[2];

#define SPEEDUP_STATUS_OFFSET 0
#define STUN_STATUS_OFFSET 1
#define PICKUP_STATUS_OFFSET 2
#define HAVE_STUN_STATUS_OFFSET 3
#define CRITICAL_READY_STAUS_OFFSET 4


#define CONTROLLER_NUM 0x0
//#define SLAVE_SER_ADDR  0x12
#define SLAVE_SER_ADDR  0x20
// number 1 use xbee 7,8
// red controller 0x12
// number 0 use xbee 4,6
// black controller 0x20

void controller_init();
void Slave_init(uint8_t slave_addr);
uint32_t Report_Process(uint8_t* cmd);
void handle_cmd(uint8_t *cmd);
void dead();
void timeup();
void speeding_timeout();
void Pick_Refresh();
void STUN_Refresh();
void pick_timeout();
void critical_reload();
void reload_in_sec();
void critical_reload_in_sec();
void game_init();


#endif
