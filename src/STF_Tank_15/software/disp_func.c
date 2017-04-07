#include "disp_func.h"
#include "control.h"
#include "oled.h"

void (*disp_func_array[10])(void);

void DispInit()
{
	int i = 0;
	for(i = 0; i < 10; ++i)
	{
		disp_func_array[i]  = 0;
	}
}

void DispDamage(void)
{
	printU16(1,0,damage_disp);
}

void DispShootEffect(void)
{
	drawString(2, 0, 4, "sho: ", 1);
	printU8(2,5,shooting_effect_disp);
}

void DispRecEffect(void)
{
	drawString(3, 0, 4, "rec: ", 1);
	printU8(3,5,rec_effect_disp);
}

void DispPickEffect(void)
{
	drawString(1, 0, 5, "pic: ", 1);
	printU8(1,5,pick_effect_disp);
}

void DispSpeed(void)
{
	printU8(0,0,left_disp);
	printU8(0,4,right_disp);
}
