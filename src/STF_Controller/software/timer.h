/*
 * timer.h
 *
 *  Created on: Apr 5, 2016
 *      Author: haoyifan
 */

#ifndef TIMER_H_
#define TIMER_H_

#define SECOND 100000000

/*
 *declare function pointer type
 */
typedef void (*handler_t)(void);

/*
 *structure holding virtual timer info
 *this may vary depending on your implementation
 */
typedef struct Timer {
    handler_t   handler;//function pointer (called after timer period)
    handler_t 	handler_in_sec;
    uint32_t    time;//time remaining for this counter.
    uint32_t 	time_in_sec; //time in seconds
    uint32_t    period;//period. 0 for one shot, non zero for periodic
    uint32_t    mode;//continuous or one shot timer. 0 for one shot, 1 for continuous
    struct Timer*  next;//points to next timer
} etimer_t;

/*
 * insert one timer into the linked list
 */
void insert_timer(etimer_t * newtimer);

//initialization
void init_hardware_timer();

/*
 * used to start a hardware timer
 */
void start_hardware_timer(uint32_t period);

/*
 * insert one time shot
 */
void startTimerOneshot(handler_t handler, handler_t handler_in_sec, uint32_t secs);


/*
 * insert continuous time shot
 */
void startTimerContinuous(handler_t handler, uint32_t period);

//pointer to start of linked list
etimer_t *rooter;



#endif /* TIMER_H_ */
