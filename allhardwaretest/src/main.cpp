#include <cstdio>
#include <cmath>

#include <bcm2835.h>
#include <unistd.h>
#include "tlc5940/tlc5940.h"
#include "tlc5940/tlc5940_pin_mapping.h"
#include "tlc5940/bcm2835_timer.h"
#include "adc/aps00b_calculation.h"
#include "mouse/mouse.h"


#define PI 3.14159

int main(int argc, char *argv[])
{
//	double x,y,Q;
	bcm2835_init();
	mouse_init();
	spi_init();
	timer_init();
	tlc_init();

	for(int i = 0; i<16; i++) {
		tlc_set_duty_cycle(i,4095);
	}
	/*for (int i=0; i<16; i++) {
		tlc_set_duty_cycle(i,0);
	}*/
	tlc_set_duty_cycle(11,3800);

	while (1) {
		/*for (int i=0; i<ADC_NUMSENSORS; i++)
			printf("ADC%d: %d\n", i, adc_samples[i]);*/

		//adc_angle();
		/*double x = adc_samples[1] - 510;
		double y = adc_samples[0] - 410;
		double Q =(180/PI)*0.5*(atan2(x,y));*/
//		printf("here1\n");
		double Q = aps00b_get_device_rotation(0);
//		printf("here2\n");
		if (Q>-45.0) {
			tlc_set_duty_cycle(10, 3800);
			tlc_set_duty_cycle(11, 4095);
		} else if (Q<-45) {
			tlc_set_duty_cycle(10, 4095);
			tlc_set_duty_cycle(11, 3800);
		}

		printf("rotated angle is: %lf\n",Q);

		printf("mouse x: %d y: %d\n",mouse_get_x(),mouse_get_y());
		/* control loop runs at 1kHz */
		usleep(1000);
	}
	tlc_destroy();
	timer_destroy();
	mouse_destroy();
	bcm2835_close();
}

