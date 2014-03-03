#ifndef __PWM_H__
#define __PWM_H__

#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#define PWM_NAME_LENGTH 20

struct pwm_data_t {
	int gpio;
	int gpio_state;
	unsigned int half_period_us;
	int enable;
      	int exit;
	pthread_t pt;
	char name[PWM_NAME_LENGTH];
};

void pwm_break(struct pwm_data_t *pwm);
void pwm_data_init(struct pwm_data_t *pwm, char *name, int gpio);
void pwm_set_frequency(struct pwm_data_t *pwm, unsigned int frequency);
void pwm_toggle(struct pwm_data_t *pwm);
void pwm_create(struct pwm_data_t *pwm, char *name, int gpio);
void pwm_remove(struct pwm_data_t *pwm);
#endif /* __PWM_H__ */
