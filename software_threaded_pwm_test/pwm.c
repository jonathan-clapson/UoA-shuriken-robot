#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#include <string.h>

#include <bcm2835.h>
#include "pwm.h"
/*struct pwm_data_t {
	int gpio;
	int gpio_state;
	int half_period_us;
	int enable;
      	int exit;
	pthread_t pt;
};*/

void pwm_break(struct pwm_data_t *pwm)
{
	pwm->gpio_state = 0;
	pwm->enable = 0;

}

void pwm_toggle(struct pwm_data_t *pwm)
{
	
	pwm->gpio_state = (pwm->gpio_state==HIGH) ? LOW:HIGH;
	bcm2835_gpio_write(pwm->gpio,pwm->gpio_state);
	//printf("gpio %d working\n %d",pwm->gpio, pwm->gpio_state);
}
/*
void pwm_toggle(struct pwm_data_t *pwm)
{
	pwm->gpio_state = !pwm->gpio_state;

	printf("%s: State is: %d\n", pwm->name, pwm->gpio_state);
}
*/
/**
 * @brief Initialse a pwm_data_t structure
 *
 * This function initialises a pwm_data_t structure. It ensures that the pwm starts disabled.
 * @param pwm a pwm_data_t structure that needs to be initialised
 * @param name a character array to copy to a pwm_data_t structure, to be used as a textual name for this pwm
 * @param gpio the gpio that will be used to produce a pwm signal
 * @return This function has no return value
 */
void pwm_data_init(struct pwm_data_t *pwm, char *name, int gpio)
{
	pwm->gpio = gpio;
	pwm->gpio_state = LOW;
	pwm->half_period_us = 0;
	pwm->enable = 0;
	pwm->exit = 0;
	strncpy(pwm->name, name, PWM_NAME_LENGTH);
}
/**
*@brief work out half period of pwm
*
*This function works out half period of pwm with given frequency. It also ensures enable the pwm when its called
*@param frequency frequency of pwm signal
*@half_period_us half period of produced pwm signal in micosecond
*@enable enable the creation of pwm signal
*@return this function returns half_period_us and enable = 1
*/
void pwm_set_frequency(struct pwm_data_t *pwm, unsigned int frequency)
{
	pwm->half_period_us = (1000000/frequency)/2;
	pwm->enable = 1;
}
/**
*@brief this function is part of pwm_create
*
*This function will be excuted when pwm_create is called and generate pwm signal
*@param 
*@return
*/
void *pwm_thread (void *arg)
{
	struct pwm_data_t *pwm = (struct pwm_data_t *) arg;

	/* main pwm loop */
	while (!pwm->exit) {
		usleep(pwm->half_period_us);
		
		if (pwm->enable && pwm->half_period_us)
			pwm_toggle(pwm);
		else
			pwm_break(pwm);

	}

	return NULL;
}
/**
*@brief this function creates pwm signal
*
*This function creates pwm signal
*@param name the name of pwm (eg: pwm_reverse or pwm_forward)
*@param gpio the gpio that will be used to produce a pwm signal
*@return
*/
void pwm_create(struct pwm_data_t *pwm, char *name, int gpio)
{
	pwm_data_init(pwm, name, gpio);
	bcm2835_gpio_fsel(gpio,BCM2835_GPIO_FSEL_OUTP);
	pthread_create(&(pwm->pt), NULL, pwm_thread, (void *) pwm);

	struct sched_param param;
	memset(&param, 0, sizeof(param));
	param.sched_priority = 40;
	pthread_setschedparam(pwm->pt, SCHED_FIFO, &param);
}

void pwm_remove(struct pwm_data_t *pwm)
{
	int *pwm_return;

	pwm->exit = 1;
	pthread_join(pwm->pt, (void **) &pwm_return);
	if (!pwm_return)
		printf("%s closed successfully\n", pwm->name);
}
