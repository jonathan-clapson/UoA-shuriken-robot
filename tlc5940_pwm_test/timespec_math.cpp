#include "timespec_math.h"

/**
 * Converts a uint16_t holding a number of microseconds into a timespec
 *
 *
 */
void microsec_to_timespec(uint64_t microsec, struct timespec *ts)
{
	ts->tv_nsec = (microsec%1000000)*1000;
	ts->tv_sec = microsec/1000000;
}

/**
 * compares timespecs for greater than, less than and equality
 *
 * This function takes two timespecs and compares returning greater than less than or equality
 * @param[in] ts1 First timespec
 * @param[in] ts2 Second timespec
 * @return Returns TIMESPEC_LESS when ts1 is less than ts2
 * TIMESPEC_GREATERS when ts1 is greater than ts2
 * TIMESPEC_EQUAL when ts1 is equal to ts2
 */
 int timespec_compare(struct timespec ts1, struct timespec ts2)
{
	if (ts1.tv_sec < ts2.tv_sec)
		return TIMESPEC_LESS;
	if (ts1.tv_sec > ts2.tv_sec)
		return TIMESPEC_GREATER;
	if (ts1.tv_nsec < ts2.tv_nsec)
		return TIMESPEC_LESS;
	if (ts1.tv_nsec > ts2.tv_nsec)
		return TIMESPEC_GREATER;
	return TIMESPEC_EQUAL;
}

/**
 * blocks for an amount of time
 */
void timespec_block_for_time(struct timespec block_time)
{
	struct timespec current_time;
	struct timespec set_time;

	/* get current time */
	clock_gettime(CLOCK_REALTIME, &current_time);

	/* determine when time should expire */
	timespec_add(current_time, block_time, &set_time);

	/* spin :D */
	while (timespec_compare(current_time, set_time) == TIMESPEC_LESS ) {
		clock_gettime(CLOCK_REALTIME, &current_time);
	};
}

/**
 * Subtracts two timespec
 *
 * Subtracts ts2 from ts1
 * @param[in] ts1 First timespec
 * @param[in] ts2 Second timespec
 * @param[out] result diffence in time as a timespec
 * @return Returns 0
 */
int timespec_subtract(struct timespec ts1, struct timespec ts2, struct timespec *result)
{
	if ( (ts1.tv_sec < ts2.tv_sec) || (ts1.tv_sec == ts2.tv_sec && (ts1.tv_nsec < ts2.tv_nsec) ) ) {
		return -1;
	}

	result->tv_sec = ts1.tv_sec - ts2.tv_sec;

	if (ts1.tv_nsec < ts2.tv_nsec) {
		ts1.tv_nsec += 1000000000L;
		(result->tv_sec)--;
	}
	result->tv_nsec = ts1.tv_nsec - ts2.tv_nsec;

	return 0;
}

/**
 * Add two timespec
 *
 * Adds ts1 and ts2
 * @param[in] ts1 First timespec
 * @param[in] ts2 Second timespec
 * @param[out] result The addition of the two timespecs
 * @return Returns 0
 */
int timespec_add(struct timespec ts1, struct timespec ts2, struct timespec *result)
{
	/* add nanoseconds */
	result->tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
	/* add seconds */
	result->tv_sec = ts1.tv_sec + ts2.tv_sec;

	/* check for seconds in nano seconds */
	if (result->tv_nsec > 1000000000L) {
		result->tv_nsec -= 1000000000L;
		result->tv_sec++;
	}

	/* TODO: should check for overflow */

	return 0;
}


