#ifndef __BCM2835_TIMER_H__
#define __BCM2835_TIMER_H__

#include <stdint.h>

#define TIMER_ERR_SUCCESS 0
#define TIMER_ERR_OPEN -1
#define TIMER_ERR_MAPPING -2

int timer_init(); 
uint64_t timer_get_time();
int timer_delay(int delay_us);
int timer_destroy();

#endif /* __BCM2835_TIMER_H__ */
