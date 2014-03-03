#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/input.h>
#include <fcntl.h>

#define MOUSEFILE "/dev/input/by-id/usb-GASIA_PS2toUSB_Adapter-event-mouse"

int main(int argc, char *argv[])
{
	struct input_event ie;

	int fd = open(MOUSEFILE, O_RDONLY);

	if (fd == -1) {
		perror("Failed to open mouse device "MOUSEFILE"\n");
		return -1;
	}

	while ( read( fd, &ie, sizeof(struct input_event) ) ) {
		if (ie.type != EV_REL)	continue;

		if (ie.code == REL_X) {
			printf("x-move\ttime %ld.%06ld\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.value);
		} else if (ie.code == REL_Y) {
			printf("y-move\ttime %ld.%06ld\tvalue %d\n", ie.time.tv_sec, ie.time.tv_usec, ie.value);
		}
	}

	return 0;
}
