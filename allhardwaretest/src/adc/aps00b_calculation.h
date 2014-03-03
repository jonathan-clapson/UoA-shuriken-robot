#ifndef __APS00B_CALCULATION_H__
#define __APS00B_CALCULATION_H__


#include "adcSpi.h"

#define PI 3.14159

double aps00b_get_angle(int adc_chan_a, int adc_chan_b);
double aps00b_get_device_rotation(int dev_num);

#endif /* __APS00B_CALCULATION_H__ */
