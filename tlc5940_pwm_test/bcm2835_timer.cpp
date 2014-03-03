#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

#include "bcm2835_timer.h"

#define TIMER_BASE 0x20003000
#define TIMER_OFFSET (4)

void *timer_base = MAP_FAILED;
int fd = -1;

int timer_init() 
{
	fd = open ("/dev/mem", O_RDONLY);
	if(fd == -1) {
		fprintf(stderr,"open() failed.\n");
		return TIMER_ERR_OPEN;
	}
	
	timer_base = mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, TIMER_BASE);
	
	if (timer_base == MAP_FAILED) {
		fprintf(stderr,"MAPPING failed.\n");
		return TIMER_ERR_MAPPING;
	}
	return TIMER_ERR_SUCCESS;
	
}

uint64_t timer_get_time()
{
	if (timer_base == MAP_FAILED)
		return 0;
	return *(uint64_t *)( (uint8_t *)timer_base + TIMER_OFFSET);
}

int timer_delay(int delay_us)
{
	uint64_t timer_done = timer_get_time() + delay_us;

	while(timer_get_time() < timer_done);
}

int timer_destroy()
{
	close(fd);
	return TIMER_ERR_SUCCESS;
}


