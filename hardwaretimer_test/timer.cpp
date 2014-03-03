#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define TIMER_BASE 0x20003000
#define TIMER_OFFSET (4)

int main (int argc, char *arg[])
{
	long long int t, prev, *timer; //64 bit timer

	/* open /dev/mem */

	int fd = open("/dev/mem", O_RDONLY);
	if (fd == -1 ) {
		fprintf(stderr, "open() failed.\n");
		return 255;
	}

	/* get mmap access to /dev/mem */
	void *timer_base = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, TIMER_BASE);
	if (timer_base == MAP_FAILED) {
		fprintf(stderr, "mmap() failed.\n");
		return 254;
	}

	timer = (long long int *) ( (char *) timer_base + TIMER_OFFSET);

	prev = *timer;

//	sleep(1);

	while (1) {
		t = *timer;
		printf("timer diff = %lld\r", t);
		fflush(stdout);
		prev = t;
//		sleep(1);
	}

	return 0;

}
