#ifndef __TIMESPEC_MATH_H__
#define __TIMESPEC_MATH_H__

#include <time.h>
#include <stdint.h>

#define TIMESPEC_LESS -1
#define TIMESPEC_EQUAL 0
#define TIMESPEC_GREATER 1

void microsec_to_timespec(uint64_t microsec, struct timespec *ts);

int timespec_compare(struct timespec ts1, struct timespec ts2);

//void timespec_block_for_time(struct timespec block_time);

int timespec_subtract(struct timespec ts1, struct timespec ts2, struct timespec *result);

int timespec_add(struct timespec ts1, struct timespec ts2, struct timespec *result);

#endif /* __TIMESPEC_MATH_H__ */
