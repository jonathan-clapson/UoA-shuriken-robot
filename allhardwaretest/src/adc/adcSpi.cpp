/******************************************************************************
* Adapted to Raspberry Pi by Xun Cui                                          *
******************************************************************************/
#define __USE_BSD

#include <stdio.h>
#include <bcm2835.h>
#include <stdint.h>
#include <unistd.h>
#include <endian.h> /* rpi is little endian, spi expects big endian */
#include <pthread.h>

#include "adcSpi.h"


struct adc_t {
	pthread_t thread;
	int running;
	uint16_t adc_samples[ADC_NUMSENSORS];
	int exit;
};
struct adc_t adc = {0,0,{0},0};
int spi_hardware_init()
{
	/* initialise spi device */
	bcm2835_spi_begin();

	/* configure for AD7918 */
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0,LOW);

	return ADC_ERR_SUCCESS;
}
void* adc_thread(void *ptr)
{
	while(!adc.exit){
		adc_update_samples();

		/* 1kHz sampling rate */
		usleep(1000);
	}
	return (void *) 0;
}
int adc_thread_create()
{
	if (adc.running == 1) {
		fprintf(stderr, "adc: thread already exists\n");
		return ADC_ERR_THREAD_EXISTS;
	}

	int thread_ret = pthread_create(&adc.thread, NULL, adc_thread, NULL);

	if (thread_ret != 0) {
		adc.running = 0;
		fprintf(stderr, "adc: Failed to create tlc_thread ERR: %d\n", thread_ret);
		return ADC_ERR_THREAD_CREATE;
	}

	fprintf(stderr, "adc: Thread created\n");
	return ADC_ERR_SUCCESS;
}

int spi_init()
{
	spi_hardware_init();
	adc_reset();
	adc_thread_create();
	
}

int spi_close()
{
	bcm2835_spi_end();
	return ADC_ERR_SUCCESS;
}

int adc_reset()
{
	uint16_t miso = htobe16(0x0000);
	uint16_t mosi = htobe16(0xFFFF);

	bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

	mosi = htobe16(0xFFFF);

	bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

	return ADC_ERR_SUCCESS;
}

int adc_update_samples()
{
	int i;
	uint16_t mosi;
	uint16_t miso;

	for(i = 0; i < ADC_NUMSENSORS; i++)
	{
//		printf("i is %d\n", i);
		mosi = htobe16(ADC_POLL(i));
		bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

		miso = be16toh(miso);
//		printf("miso%d: %x\n", i, miso);

		//bcm2835_spi_transfernb(&adc_wdata,&adc_rdata, sizeof(uint16_t)); 

		int channel_num = ADC_GETADDR(miso);
		int reading = ADC_GETLVL(miso);
//		printf("chan%d: %u\n", channel_num, reading);
		adc.adc_samples[channel_num] = reading;
	}
	return ADC_ERR_SUCCESS;
}
/*double adc_angle(double x,double y)
{
	x = adc_samples[1];
	y = adc_samples[0];
	double Q = 0.5*atan((double x)/(double y));

}*/
uint16_t adc_get_reading(int channel_num)
{
	return adc.adc_samples[channel_num];
}
int adc_thread_destroy()
{
	if (adc.running == 0) {
	fprintf(stderr, "adc: thread does not exist\n");
		return ADC_ERR_THREAD_RELEASE;
	}

	adc.exit = 1;
	pthread_join(adc.thread, NULL);//wait for thread to finish
	adc.exit = 0;

	adc.running = 0;

	return ADC_ERR_SUCCESS;
}

int adc_destroy()
{

	/* destroy adc thread */
	adc_thread_destroy();

	bcm2835_spi_end();

	return ADC_ERR_SUCCESS;
}
