#ifndef __TLC5940_H__

#define TLC_NUM_CHANNELS 16
#define TLC_BITS_PER_CHANNEL 12
#define TLC_12_BIT_MASK 0x0FFF;

#define TLC_ERR_SUCCESS 0
#define TLC_ERR_OUT_OF_RANGE -1
#define TLC_ERR_THREAD_CREATE -2
#define TLC_ERR_THREAD_RELEASE -3
#define TLC_ERR_THREAD_EXISTS -4

void tlc_set_duty_cycle(unsigned int channel_id, unsigned level);

int tlc_init();
int tlc_destroy();

#endif /* __TLC5940_H__ */

