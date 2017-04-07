#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "drivers/mss_gpio/mss_gpio.h"
#include "communication.h"

#define SHOOT_ADDR 	(volatile uint32_t*)0x40050800
#define LEFT_ADDR 	(volatile uint32_t*)0x40050100
#define RIGHT_ADDR 	(volatile uint32_t*)0x40050200
#define SERVO_ADDR 	(volatile uint32_t*)0x40050300
#define AUDIO_ADDR 	(volatile uint32_t*)0x40050400
#define HARM_ADDR	(volatile uint32_t*)0x40050800
#define RAND_ADDR	(volatile uint32_t*)0x40050600
#define LED_ADDR	(volatile uint32_t*)0x40050700

#define SHOOT_SOUND 0x0000
#define CRITICAL_SHOOT_SOUND 0x8000
#define SHOT_SOUND 0x4000
#define STUNNED_SOUND 0xC000
#define HEALING_SOUND 0x2000
#define PICK1_SOUND 0xA000
#define PICK2_SOUND 0x6000
#define DEAD_SOUND 0xE000
#define WIN_SOUND 0x1000

#define UPPER_BOUND 230000
#define LOWER_BOUND 70000

extern uint32_t pulseWidth;
#define PWM_PERIOD 2000000 //motor max

#define LEFT_FORWARD	0x2 << 2
#define LEFT_BACKWARD	0x1 << 2
#define LEFT_STOP		0x3 << 2
#define RIGHT_FORWARD	0x2 << 4
#define RIGHT_BACKWARD	0x1 << 4
#define RIGHT_STOP		0x3 << 4

#define SHOOTING_ADDR_BIT_OFFSET 2

#define CMD_REPORT_TYPE 0
#define CMD_RESPONSE_TYPE 1

//cmd_effect types
#define EFFECT_SPEED_UP 0x01
#define EFFECT_HEALING 0x02
#define EFFECT_STUN 0x03

#define CMD_TYPE_BYTE_OFFSET 0
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

#define CTRL_TYPE_BYTE_OFFSET 0
#define CTRL_LEFT_MOTOR_BYTE_OFFSET 1
#define CTRL_RIGHT_MOTOR_BYTE_OFFSET 2
#define CTRL_SERVO_BYTE_OFFSET 3
#define CTRL_SHOOT_BYTE_OFFSET 4
//#define CTRL_PICK_BYTE_OFFSET 5 //pick package
#define CTRL_SEQ_BYTE_OFFSET 5

#define CTRL_SEQ_INIT_BYTE_OFFSET 1
#define CTRL_MASTER_ADDR_BYTE_OFFSET 2
#define CTRL_SELF_ADDR_BYTE_OFFSET 3
#define CTRL_ENEMY_ADDR_BYTE_OFFSET 4

#define CTRL_TYPE_CTRL 0
#define CTRL_TYPE_INIT 1
#define CTRL_TYPE_DEAD 2

#define NORMAL_DAMAGE 0x00
#define CRITICAL_DAMAGE 0x01
#define STUN 0x02
#define EXPLOSIVE 0x03

#define SPEED_UP1			169
#define HEALING1			181
#define STUN1				228
#define SPEED_UP2			180
#define HEALING2			84
#define STUN2				20

#define DAMAGE 200

#define MY_DEBUG 0
#define CRY 1
#define SMILE 2

extern uint8_t disp_num;
extern uint16_t damage_disp;
extern uint16_t shooting_effect_disp;
extern uint16_t rec_effect_disp;
extern uint8_t pick_effect_disp;
extern uint8_t left_disp;
extern uint8_t right_disp;

uint32_t Tank_Act(uint8_t* cmd);
void control_init();

#endif
