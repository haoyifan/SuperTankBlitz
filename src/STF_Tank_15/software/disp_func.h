#ifndef _DSIP_FUNC_
#define _DSIP_FUNC_

extern void (*disp_func_array[10])(void);

void DispInit();

void DispDamage(void);
void DispShootEffect(void);
void DispRecEffect(void);
void DispPickEffect(void);
void DispSpeed(void);

#endif
