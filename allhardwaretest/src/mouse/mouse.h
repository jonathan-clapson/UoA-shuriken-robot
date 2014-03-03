#ifndef __MOUSE_H__
#define __MOUSE_H__

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

#include <linux/input.h>
#include <fcntl.h>

#define MOUSEFILE "/dev/input/by-id/usb-GASIA_PS2toUSB_Adapter-event-mouse"

#define MOUSE_ERR_SUCCESS 0
#define MOUSE_ERR_THREAD_CREATE -1
#define MOUSE_ERR_THREAD_EXISTS -2
#define MOUSE_ERR_OPEN -3
#define MOUSE_ERR_THREAD_RELEASE -4

int mouse_get_x();
int mouse_get_y();

int mouse_init();
int mouse_destroy();

#endif /* __MOUSE_H__ */
