/*
 * timer.c
 *
 *  Created on: Apr 5, 2016
 *      Author: haoyifan
 */

#include "drivers/mss_timer/mss_timer.h"
#include "drivers/mss_gpio/mss_gpio.h"
#include <stdlib.h>
#include <stdio.h>
#include "timer.h"

void startTimerOneshot(handler_t handler, handler_t handler_in_sec, uint32_t secs)
{
    etimer_t * node = malloc(sizeof(etimer_t));
    node->handler = handler;
    node->handler_in_sec = handler_in_sec;
    node->time_in_sec = secs;
    node->time = SECOND;
    node->period = 0;
    node->mode = 0;
    node->next = NULL;

    //printf("start timer: %d\r\n", node->time_in_sec);
    //update and start first timer if needed
    etimer_t * temper = rooter;
    if (temper == NULL){
    	insert_timer(node);
		start_hardware_timer(rooter->time);
		return;
    }
    uint32_t current_count = rooter->time - MSS_TIM1_get_current_value();
	while(temper != NULL)
	{
		temper->time = temper->time - current_count;
		temper = temper->next;
	}

    insert_timer(node);
    if (node == rooter)
    {
    	start_hardware_timer(rooter->time);
    }
}
/*
void startTimerContinuous(handler_t handler, uint32_t period)
{
    etimer_t * node = malloc(sizeof(etimer_t));
    node->handler = handler;
    node->time = period;
    node->period = period;
    node->mode = 1;
    node->next = NULL;

    insert_timer(node);
}*/

void insert_timer(etimer_t * newtimer)
{
	if(rooter == NULL)
	{
		rooter = newtimer;
		rooter->next = NULL;
		return;
	}
	etimer_t* temp = rooter->next;
    etimer_t* temp_b = rooter;
    if(temp == NULL){
        if(rooter->time < newtimer->time){
            rooter->next = newtimer;
            newtimer->next = NULL;
            return;
        }
        else{
            newtimer->next = rooter;
            rooter = newtimer;
            return;
        }
    }
    if(rooter->time > newtimer->time){
        newtimer->next = rooter;
        rooter = newtimer;
        return;
    }
    while(temp != NULL){
        if(temp->time < newtimer->time){
            temp = temp->next;
            temp_b = temp_b->next;
            continue;
        }
        else{
            temp_b->next = newtimer;
            newtimer->next = temp;
            return;
        }
    }
    temp_b->next = newtimer;
    newtimer->next = NULL;
}


void init_hardware_timer(){
	MSS_TIM1_init(MSS_TIMER_ONE_SHOT_MODE);
}

//look in mss_timer.h for details
void start_hardware_timer(uint32_t period){
	MSS_TIM1_load_immediate(period);
	MSS_TIM1_start();
	MSS_TIM1_enable_irq();
}


void Timer1_IRQHandler( void ){ //TODO: handle large time

    uint32_t current_count = rooter->time;
    uint32_t update = 0;

    if(rooter == NULL)
        return;

    while ((update == 0 && rooter->time == current_count)||(update == 1 && rooter->time == 0)) {
        //if (update) printf("updated once, this timer has time_in_sec: %d.\n\r", rooter->time_in_sec);
        //else printf("this is head, this timer has time_in_sec: %d.\n\r", rooter->time_in_sec);
        rooter->time_in_sec -= 1;
        //printf("timer left time_in_sec: %d\n\r", rooter->time_in_sec);
        if (rooter->time_in_sec == 0)
        	rooter->handler(); // execute the corresponding function
        else if (rooter->handler_in_sec != NULL)
        	rooter->handler_in_sec();
        etimer_t * temp = rooter;

        rooter = rooter->next;
        if(update == 0)
        {
        	etimer_t * temper = rooter;
        	while(temper != NULL)
        	{
        		temper->time = temper->time - current_count;
        		temper = temper->next;
        	}
        	update = 1;
        }
        if (temp->mode == 0) {
            // one shot mode
        	if (temp->time_in_sec > 0)
        	{
        		temp->time = SECOND;
        		insert_timer(temp);
        	}
        	else
        	{
        		temp->next = NULL;
        		free(temp);
        	}
        }
        else //not maintained
        {
            // continuous mode
            temp->time = temp->period;
            insert_timer(temp);
        }
        if(rooter == NULL)
        {
            MSS_TIM1_clear_irq();
            //MSS_TIM1_stop();
            return;
        }
    }
    current_count = rooter->time;
	MSS_TIM1_clear_irq();
    MSS_TIM1_load_immediate(current_count);
    MSS_TIM1_start();
}
