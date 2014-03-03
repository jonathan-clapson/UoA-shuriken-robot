#include <bcm2835.h>
#include <stdio.h>
#include <math.h>
#define PI 3.14159

int main(int argc, char *argv[])
{
	bcm2835_init();

	bcm2835_spi_begin();

	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

	bcm2835_gpio_fsel(RPI_V2_GPIO_P1_13, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(RPI_V2_GPIO_P1_13, LOW);

	uint8_t buf;
	uint8_t rec;

for (int j=0; j<2; j++){

	for (int i=0; i<(24/8); i++) {
/*		if ((i%3) == 0){
			buf = 128;
		}
		else if ((i%3) == 1) {
			buf = 8;
		}
		else if ((i%3) == 2) {
			buf = 0;
		}*/
		buf = 0;
		bcm2835_spi_transfernb(&buf, &rec, sizeof(rec));
	}

	bcm2835_gpio_write(RPI_V2_GPIO_P1_13, HIGH);
	bcm2835_delay(1);
	bcm2835_gpio_write(RPI_V2_GPIO_P1_13, LOW);
	bcm2835_delay(1);
}

	bcm2835_close();
}

