#ifndef __ROBOT_ADC_SPI_H__
#define __ROBOT_ADC_SPI_H__

#include <stdint.h>

#define ADC_ERR_SUCCESS 0
#define ADC_ERR_THREAD_EXISTS -1
#define ADC_ERR_THREAD_CREATE -2
#define ADC_ERR_THREAD_RELEASE -3
// Contains values for control register
#define ADC_CODING 0x01
#define ADC_RANGE 0x02
// Define shadow. Used in conjunction with seq bit.
#define ADC_SHADOW 0x08
// Define power management bits
#define ADC_PM0 0x10
#define ADC_PM1 0x20
#define ADC_ADD0 0x40
#define ADC_ADD1 0x80
#define ADC_ADD2 0x100
#define ADC_SEQ 0x400
#define ADC_WRITE 0x800

// Define seq bit. Determines whether pins are read in a set order
#define ADC_SEQINIT ADC_SHADOW
#define ADC_SEQRUN ADC_SEQ
#define ADC_SEQSET ADC_SEQ|ADC_SHADOW

#define ADC_ADDR(addr)         (addr<<6)

#define ADC_PM_NORM ADC_PM0|ADC_PM1
#define ADC_PM_FULLSHUT ADC_PM1
#define ADC_PM_AUTOSHUT ADC_PM0

#define ADC_POLL(addr) ((ADC_WRITE|ADC_ADDR(addr)|ADC_PM_NORM|ADC_CODING)<<4)

#define ADC_GETADDR(val) ((val&0x7000)>>12)
#define ADC_GETLVL(val) ((val&0x0FFC)>>2)

#define ADC_NUMSENSORS 8

int spi_init();
int spi_close();
int spi_hardware_init();
void* adc_thread(void *ptr);
int adc_thread_create();
int adc_reset();
int adc_update_samples();
uint16_t adc_get_reading(int channel_num);
int adc_thread_destroy();
int adc_destroy();



#endif
