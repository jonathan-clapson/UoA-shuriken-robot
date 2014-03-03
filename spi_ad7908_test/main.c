#include <bcm2835.h>
#include <stdio.h>
#include <math.h>
#include "adcSpi.h"
#define PI 3.14159

int main(int argc, char *argv[])
{
	double x,y,Q;
	bcm2835_init();
	
	spi_init();
	
	adc_reset();

	while (1) {
		adc_update_samples();
	
		for (int i=0; i<ADC_NUMSENSORS; i++)
			
				printf("ADC%d: %d\n", i, adc_samples[i]);
			
		//adc_angle();
		double x = adc_samples[1] - 510;
		double y = adc_samples[0] - 410;
		double Q =(180/PI)*0.5*(atan2(x,y));
		printf("rotated angle is: %lf\n",Q);

		sleep(1);
	}


	bcm2835_close();
}

