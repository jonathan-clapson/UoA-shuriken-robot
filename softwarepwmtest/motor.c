#include <stdio.h>

#include "pwm.h"

struct motor_data_t {
	struct pwm_data_t pwm[2];
	
};

void motor_create(struct motor_data_t *motor, int gpio_forward, int gpio_reverse)
{

	pwm_create( &(motor->pwm[0]), "pwm_forward", gpio_forward );
	pwm_create( &(motor->pwm[1]), "pwm_reverse", gpio_reverse );

}

/* this function needs a translation from speed to frequency/period */
void motor_set_speed(struct motor_data_t *motor, int speed)
{
	if (speed == 0) {
		pwm_break( &(motor->pwm[0]) );
		pwm_break( &(motor->pwm[1]) );
	} else if (speed > 0) {
		pwm_set_frequency( &(motor->pwm[0]), speed );
		pwm_break( &(motor->pwm[1]) );
	} else {
		pwm_set_frequency( &(motor->pwm[1]), -speed );
		pwm_break( &(motor->pwm[0]) );
	}
}

void motor_remove(struct motor_data_t *motor)
{
	pwm_remove( &(motor->pwm[0]) );
	pwm_remove( &(motor->pwm[1]) );
	printf("Motor removed successfully\n");
}
