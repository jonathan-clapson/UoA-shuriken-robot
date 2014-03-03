#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <pthread.h>

#include <linux/input.h>
#include <fcntl.h>

#include "mouse.h"

struct mouse_t {
	pthread_t thread;
	int running;
	int x, y;
	int exit;
	int fd;
};
struct mouse_t mouse = { 0, 0, 0, 0, 0 };

int mouse_open()
{
	mouse.fd = open(MOUSEFILE, O_RDONLY);

	if (mouse.fd == -1) {
		fprintf(stderr, "Failed to open mouse device %s\n", MOUSEFILE);
		return MOUSE_ERR_OPEN;
	}
	return MOUSE_ERR_SUCCESS;
}

int mouse_get_x()
{
	return mouse.x;
}
int mouse_get_y()
{
	return mouse.y;
}

void* mouse_thread(void *ptr)
{
	struct input_event ie;

	while (!mouse.exit) {
		read (mouse.fd, &ie, sizeof(struct input_event) );
		if (ie.type != EV_REL)	continue;

		if (ie.code == REL_X) {
			mouse.x -= ie.value;
		} else if (ie.code == REL_Y) {
			mouse.y += ie.value;
		}
		/* mouse poll rate usually 125Hz. This means period is 8000us. However sleep sometimes delays so lets make it half of that 
		 * reducing chance of missing reading
		 */
		usleep(4000);
	}

	return 0;
}

int mouse_thread_create()
{
	if (mouse.running == 1) {
		fprintf(stderr, "mouse: thread already exists\n");
		return MOUSE_ERR_THREAD_EXISTS;
	}

	int thread_ret = pthread_create(&mouse.thread, NULL, mouse_thread, NULL);

	if (thread_ret != 0) {
		mouse.running = 0;
		fprintf(stderr, "mouse: Failed to create tlc_thread ERR: %d\n", thread_ret);
		return MOUSE_ERR_THREAD_CREATE;
	}

	fprintf(stderr, "mouse: Thread created\n");
	return MOUSE_ERR_SUCCESS;
}

int mouse_init()
{
	/* open mouse device */
	mouse_open();

	/* create thread which tracks mouse position */
	mouse_thread_create();

	return MOUSE_ERR_SUCCESS;
}
int mouse_thread_destroy()
{
	if (mouse.running == 0) {
	fprintf(stderr, "mouse: thread does not exist\n");
		return MOUSE_ERR_THREAD_RELEASE;
	}

	mouse.exit = 1;
	pthread_join(mouse.thread, NULL);
	mouse.exit = 0;

	mouse.running = 0;

	return MOUSE_ERR_SUCCESS;
}

int mouse_destroy()
{

	/* destroy mouse thread */
	mouse_thread_destroy();

	/* close mouse device */
	close(mouse.fd);

	return MOUSE_ERR_SUCCESS;
}
