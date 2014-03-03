#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>

#include "tlc5940.h"
#include "tlc5940_pin_mapping.h"

#include "bcm2835_timer.h"

int main(int argc, char *argv[])
{
	bcm2835_init();

	timer_init();

	tlc_init();

	printf("got back!\n");
	fflush (stdout);

	for (int i=0; i<16; i++)
		tlc_set_duty_cycle(i, 4095);

	//tlc_pack_send_buffer();
	//tlc_update_gpio();

//	sleep(15);
	usleep(5000);

//	for (int i=0; i<16; i++)
//		tlc_set_duty_cycle(i, 2048);
	tlc_set_duty_cycle(15, 2048);

	usleep(5000);

	printf("Run successful\n");

	tlc_destroy();

	timer_destroy();

	bcm2835_close();

	return 0;
}
