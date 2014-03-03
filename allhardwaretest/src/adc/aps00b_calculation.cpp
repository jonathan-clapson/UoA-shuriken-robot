#include <cmath>
#include <cstdio>

#include "adcSpi.h"
#include "aps00b_calculation.h"

#define PI 3.14159

#define APS_NUM_DEVICES 3

int aps_mapping[APS_NUM_DEVICES][2] = { {1,0}, {3,2}, {5,4} };

double aps00b_get_angle(int adc_chan_a, int adc_chan_b)
{
	//FIXME: safety check this
	//adc_update_samples();

	double x = adc_get_reading(adc_chan_a) - 510;
	double y = adc_get_reading(adc_chan_b) - 410;
	double Q = (180/PI)*0.5*(atan2(x,y));

	return Q; 
}

double aps00b_get_device_rotation(int dev_num)
{
	if ( (dev_num < 0) || (dev_num > APS_NUM_DEVICES-1) )
		return 0.0f;

	return aps00b_get_angle(aps_mapping[dev_num][0], aps_mapping[dev_num][1]);
}
