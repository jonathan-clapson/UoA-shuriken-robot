#include <pthread.h>
#include <bcm2835.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include <unistd.h>

#include "timespec_math.h"

#include "tlc5940.h"
#include "tlc5940_pin_mapping.h"
#include "bcm2835_timer.h"

struct tlc_struct {
	pthread_t thread;
	pthread_mutex_t mutex;
	int running;
	int exit;
	/* the buffer to modify io's in */
	unsigned int duty_cycles[TLC_NUM_CHANNELS];
	int duty_cycles_changed;
};
struct tlc_struct tlc = { 0, PTHREAD_MUTEX_INITIALIZER, 0, 0, {0}, 0 };

/* the buffer that is used for sending updated gpio info */
uint8_t tlc_send_buffer[TLC_NUM_CHANNELS*TLC_BITS_PER_CHANNEL/8] = {0};

int tlc_send_bit(uint8_t bit)
{
	struct timespec one_us;
	one_us.tv_nsec = 1000;

	if (bit != 0 && bit !=1)
		return TLC_ERR_OUT_OF_RANGE;

	uint8_t send_bit = (bit) ? HIGH : LOW;
	bcm2835_gpio_write(TLC_SIN, send_bit);

	//timespec_block_for_time(one_us);
	timer_delay(1);

	bcm2835_gpio_write(TLC_SCLK, HIGH);

	//timespec_block_for_time(one_us);
	timer_delay(1);
	bcm2835_gpio_write(TLC_SCLK, LOW);

	//timespec_block_for_time(one_us);
	timer_delay(1);
	return TLC_ERR_SUCCESS;
}

void tlc_pack_send_buffer(unsigned int duty_cycle[TLC_NUM_CHANNELS])
{
	char more_flag = 0;
	uint16_t buff12 = 0;
	uint8_t buff8 = 0;
	char send_buffer_index = 0;

/*	printf("am here\n");
	fflush(stdout);*/

	for (int i=0; i<TLC_NUM_CHANNELS; i++) {
/*		printf("am here %d\n", i);
		fflush(stdout);*/

		if (more_flag)  {
			/* deal with remainder of last byte */
			buff8 = (buff12 & 0xF) << 4;
			buff12 = duty_cycle[i] & TLC_12_BIT_MASK;
			buff8 |= (buff12 & 0x0F00) >> 8;

			tlc_send_buffer[send_buffer_index] = buff8;
			send_buffer_index++;
			more_flag = 0;
			printf("iter%d: %d\n", i, buff8);

			/* deal with rest of current byte */
			buff8 = buff12 & 0xFF;
			tlc_send_buffer[send_buffer_index] = buff8;
			send_buffer_index++;
			printf("iter%d: %d\n", i, buff8);
		} else {
			/* deal with current byte, this will have remainder */
			buff12 = duty_cycle[i] & TLC_12_BIT_MASK;
			buff8 = (buff12 >> 4);

			tlc_send_buffer[send_buffer_index] = buff8;
			send_buffer_index++;
			more_flag = 1;
			printf("iter%d: %d\n", i, buff8);
		}

	}
	tlc.duty_cycles_changed = 1;
}

void tlc_set_duty_cycle(unsigned int channel_id, unsigned int level)
{
	static int count = 0;
	printf("duty_cycle: update %d\n", count++);
/*	printf("duty_cycle: attempting to lock mutex\n");
	fflush(stdout);*/
	pthread_mutex_lock( &(tlc.mutex) );
/*	printf("duty_cycle: locked mutex\n");
	fflush(stdout);*/
	tlc.duty_cycles[channel_id] = level;
	tlc_pack_send_buffer(tlc.duty_cycles);
	pthread_mutex_unlock( &(tlc.mutex) );
/*	printf("set channel %u to: %u\nduty_cycle: mutex unlocked\n", channel_id, level);
	fflush(stdout);*/
}

int tlc_update_gpio()
{
	int err_ret = TLC_ERR_SUCCESS;
	pthread_mutex_lock( &(tlc.mutex) );
	for (int i=0; i<sizeof(tlc_send_buffer); i++) {
		uint8_t byte = tlc_send_buffer[i];

		for (int bit_num=7; bit_num>=0; bit_num--) {
			uint8_t bit = (byte >> bit_num) & 0x1;
			//printf("byte%d:%d: %d\n", i, byte, bit);

			int ret = tlc_send_bit(bit);
			if (ret < 0)
				err_ret = ret;
		}
	}
	pthread_mutex_unlock( &(tlc.mutex) );

	bcm2835_gpio_write(TLC_XLAT, HIGH);
	usleep(1);
	bcm2835_gpio_write(TLC_XLAT, LOW);

	/*//disable blank
	bcm2835_gpio_write(TLC_BLANK, LOW);

	for (int i=0; i<4095; i++) {//is this correct?? maybe should be 4096?
		tlc_toggle_clock();
	}

	//ensure blank is set back on
	bcm2835_gpio_write(TLC_BLANK, HIGH);

	//disable blank
	bcm2835_gpio_write(TLC_BLANK, LOW);

	for (int i=0; i<4095; i++) {//is this correct?? maybe should be 4096?
		tlc_toggle_clock();
	}

	//ensure blank is set back on
	bcm2835_gpio_write(TLC_BLANK, HIGH);*/

	return err_ret;
}


/* 
 * old decrepid function probably never to be used again
 */
int tlc_toggle_clock()
{
//	static int i=0;
	bcm2835_gpio_write(TLC_GSCLK, HIGH);
	bcm2835_delay(1);
	bcm2835_gpio_write(TLC_GSCLK, LOW);
	bcm2835_delay(1);

//	printf("i: %d\n", i);
//	i++;
}

int tlc_pin_init() 
{
	/* Configure pins for correct modes */
	bcm2835_gpio_fsel(TLC_VPRG, BCM2835_GPIO_FSEL_OUTP);
#ifdef TLC_CARE_ERROR
	bcm2835_gpio_fsel(TLC_XERR, BCM2835_GPIO_FSEL_INPUT);
#endif /* TLC_CARE_ERROR */
	/* GSCLK is set to INPT to disable it until it is used */
	//bcm2835_gpio_fsel(TLC_GSCLK, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(TLC_GSCLK, BCM2835_GPIO_FSEL_ALT5);
	bcm2835_gpio_fsel(TLC_XLAT, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(TLC_BLANK, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(TLC_SIN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(TLC_SCLK, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(TLC_BLANK, BCM2835_GPIO_FSEL_OUTP);

	/* set output pins to correct initial values */
	bcm2835_gpio_write(TLC_BLANK, HIGH);
	bcm2835_gpio_write(TLC_VPRG, LOW);

	bcm2835_gpio_write(TLC_GSCLK, LOW);
	bcm2835_gpio_write(TLC_XLAT, LOW);
	bcm2835_gpio_write(TLC_SIN, LOW);
	bcm2835_gpio_write(TLC_SCLK, LOW);

#define PWM_CHANNEL 0 /* I don't think any other channel will work? */

	/* Set up PWM for GSCLK, yea this makes timing hard, but it makes it fast */
	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_2);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1 , 1);
	bcm2835_pwm_set_range(PWM_CHANNEL, 2);
	bcm2835_pwm_set_data(PWM_CHANNEL, 1);

	return 0;
}

int tlc_pin_cleanup()
{
	bcm2835_gpio_fsel(TLC_VPRG, BCM2835_GPIO_FSEL_INPT);
#ifdef TLC_CARE_ERROR
	bcm2835_gpio_fsel(TLC_XERR, BCM2835_GPIO_FSEL_INPT);
#endif /* TLC_CARE_ERROR */
	bcm2835_gpio_fsel(TLC_GSCLK, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(TLC_XLAT, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(TLC_BLANK, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(TLC_SIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_fsel(TLC_SCLK, BCM2835_GPIO_FSEL_INPT);
}

/**
 * This function seems to cause threading issues. It will only 'help' if the motors are not being run at full speed.
 * It will also probably make it harder to model whats happening with the motor mechanics, as such abondoning development for time being.
 */
int tlc_duty_cycle_subtract_time( unsigned int corrected_duty_cycle[TLC_NUM_CHANNELS], struct timespec error_offset )
{
	/* 4096 counts to 1ms; this means 1 count is 244ns 
	this means the time offset needs to be a factor of 244ns
	*/
printf("dc_sub: in\n");
fflush(stdout);
	if (error_offset.tv_sec > 0) {
		fprintf(stderr, "tlc: Offset correction found at least 1 second of error, fix it please!\n");
		return -1;
	}

	int count_offset = error_offset.tv_nsec/244;

	for (int i=0; i<TLC_NUM_CHANNELS; i++) {
		pthread_mutex_lock( &(tlc.mutex) );

		corrected_duty_cycle[i] = tlc.duty_cycles[i] - count_offset;

		printf("set: %u offset: %u\n", tlc.duty_cycles[i], count_offset);
		fflush(stdout);
		if (corrected_duty_cycle[i] > tlc.duty_cycles[i]) {
			/* TODO: maybe add a cutoff threshold to detect if there are serious issues or not? */
			//fprintf(stderr, "tlc: offset correction overflow\n");
			corrected_duty_cycle[i] = 0;
			printf("correcting %d: level %u to %u\n", i, tlc.duty_cycles[i], corrected_duty_cycle[i]);
			fflush(stdout);
		}
		pthread_mutex_unlock( &(tlc.mutex) );
//		printf("dc_sub: releasing mutex\n");
//		fflush(stdout);
	}
	return 0;
}

/*
 * Send next duty cycle command to TLC5940
 * This must be done as the device does not continuosly create the waveform
 * it only performs one period. One period has been fixed at 1ms to make code
 * simpler.
 */
void* tlc_duty_cycle_send(void * args)
{
	int ret;
	struct timespec current_time;
	struct timespec set_time;
	struct timespec error_time;
	struct timespec one_ms = {0};
	struct timespec timer;
	one_ms.tv_nsec = 1000000;
	unsigned int corrected_duty_cycle[TLC_NUM_CHANNELS];
/*	printf("going to sleep\n");
	fflush(stdout);*/
//	sleep(4);
/*	printf("woke up\n");
	fflush(stdout);*/

	int count = 0;

	/* initialise last_time */
	microsec_to_timespec(timer_get_time(), &set_time);

	while (!tlc.exit) {
		/* grab the time this function HAS been called */
		microsec_to_timespec(timer_get_time(), &current_time);
		//TODO: check ret?

		//compute differnce from when should and has been called
//		timespec_subtract(current_time, set_time, &error_time);

		//printf("error_time: %ds, %dns\n", error_time.tv_sec, error_time.tv_nsec);
		//take the error in time off to reduce error
		//tlc_duty_cycle_subtract_time(corrected_duty_cycle, error_time);

		//move data in correct format to send buffer
		//tlc_pack_send_buffer(tlc.duty_cycles);

		if (tlc.duty_cycles_changed) {
			//write out pwms to run
			tlc_update_gpio();
			tlc.duty_cycles_changed = 0;
			printf("tlc: clearing %d\n", count++);
		}

		//disable blank
		bcm2835_gpio_write(TLC_BLANK, LOW);

		//enable the pwm
		//bcm2835_gpio_fsel(TLC_GSCLK, BCM2835_GPIO_FSEL_ALT5);

		/* PWM clk source is 19.2MHz. This is divided by 2 (9.6MHz). It then goes into the pwm block, where the range is 2 and value is 1.
		 * This means each period is 2 counts, and it switches phase when it reaches 1. This again halves the frequency (4.8MHz).
		 * 1/(4.8MHz) * 4096 = 853us (4096 is the range of the tlc5940).
		 */
		//usleep for tlc5940 cycle time
		usleep(853);

		//disable the pwm
		//bcm2835_gpio_fsel(TLC_GSCLK, BCM2835_GPIO_FSEL_INPT);

		//re-blank
		bcm2835_gpio_write(TLC_BLANK, HIGH);

		/* record next time this function *should* be called */
		timespec_add(current_time, one_ms, &set_time);
//		printf("running\n");
	}

	printf("tlc5940: exiting\n");

	return (void *) 0;
}

/*
 * Initialise background thread
 */
int tlc_thread_create()
{
	if (tlc.running == 1) {
		fprintf(stderr, "tlc: thread already exists\n");
		return TLC_ERR_THREAD_EXISTS;
	}

	tlc.running = 1;
	int thread_ret = pthread_create(&tlc.thread, NULL, tlc_duty_cycle_send, NULL);
	if (thread_ret != 0) {
		tlc.running = 0;
		fprintf(stderr, "tlc: Failed to create tlc_thread ERR: %d\n", thread_ret);
		return TLC_ERR_THREAD_CREATE;
	}

	fprintf(stderr, "tlc: Thread created\n");
	return TLC_ERR_SUCCESS;
}

int tlc_thread_destroy()
{
	if (tlc.running == 0) {
		fprintf(stderr, "tlc: thread does not exist\n");
		return TLC_ERR_THREAD_RELEASE;
	}

	tlc.exit = 1;
	pthread_join(tlc.thread, NULL);
	tlc.exit = 0;

	tlc.running = 0;

	return TLC_ERR_SUCCESS;

}

int tlc_destroy()
{
	/* end thread */
	tlc_thread_destroy();

	/* return pins to safety */
	tlc_pin_cleanup();
}

int tlc_init()
{
	tlc_pin_init();
	tlc_thread_create();

	return 0;
}
