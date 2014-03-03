#ifndef __MOTOR_H__
#define __MOTOR_H__

#include <stdio.h>

#include "pwm.h"

struct motor_data_t {
	struct pwm_data_t pwm[2];
	
};

void motor_create(struct motor_data_t *motor, int gpio_forward, int gpio_reverse);

/* this function needs a translation from speed to frequency/period */
void motor_set_speed(struct motor_data_t *motor, int speed);
void motor_remove(struct motor_data_t *motor);

#endif /* __MOTOR_H__ */
