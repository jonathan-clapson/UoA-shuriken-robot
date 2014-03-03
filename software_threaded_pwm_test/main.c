#include <stdio.h>
#include <bcm2835.h>
#include "motor.h"

int main (int argc, char *argv[])
{
	struct motor_data_t motor0;
	if(!bcm2835_init())
	{
		return -1;
	}
	motor_create(&motor0, RPI_V2_GPIO_P1_11, RPI_V2_GPIO_P1_13);

	motor_set_speed(&motor0,100000);
	
	sleep(10);
	/*
	motor_set_speed(&motor0, 0);
	
	sleep(10);
	motor_set_speed(&motor0, -1);
	sleep(5);
	*/

	motor_remove(&motor0);

	return 0;
}
