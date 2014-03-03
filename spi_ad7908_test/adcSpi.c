/******************************************************************************
* Adapted to Raspberry Pi by Xun Cui                                          *
******************************************************************************/
#define __USE_BSD

#include <stdio.h>
#include <bcm2835.h>
#include <stdint.h>
#include <unistd.h>
#include <endian.h> /* rpi is little endian, spi expects big endian */

#include "adcSpi.h"
#include "errcodes.h"

uint16_t adc_samples[ADC_NUMSENSORS];

int spi_init()
{
	/* initialise spi device */
	bcm2835_spi_begin();

	/* configure for AD7918 */
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0,LOW);

	return ERR_SUCCESS;
}

int spi_close()
{
	bcm2835_spi_end();
	return ERR_SUCCESS;
}

int adc_reset()
{
	uint16_t miso = htobe16(0x0000);
	uint16_t mosi = htobe16(0xFFFF);

	bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

	mosi = htobe16(0xFFFF);

	bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

	return ERR_SUCCESS;
}

int adc_update_samples()
{
	int i;
	uint16_t mosi;
	uint16_t miso;

	for(i = 0; i < ADC_NUMSENSORS; i++)
	{
		printf("i is %d\n", i);
		mosi = htobe16(ADC_POLL(i));
		bcm2835_spi_transfernb((char *) &mosi, (char *) &miso, sizeof(miso));

		miso = be16toh(miso);
		printf("miso%d: %x\n", i, miso);

		//bcm2835_spi_transfernb(&adc_wdata,&adc_rdata, sizeof(uint16_t)); 

		int channel_num = ADC_GETADDR(miso);
		int reading = ADC_GETLVL(miso);
		printf("chan%d: %u\n", channel_num, reading);
		adc_samples[channel_num] = reading;
	}
	return ERR_SUCCESS;
}
/*double adc_angle(double x,double y)
{
	x = adc_samples[1];
	y = adc_samples[0];
	double Q = 0.5*atan((double x)/(double y));

}*/
