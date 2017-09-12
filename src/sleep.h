#ifndef __SLEEP_H__
#define __SLEEP_H__

#define NUM_SLEEP_MODES 5
typedef enum {
    EM0 = 0,
    EM1 = 1,
    EM2 = 2,
    EM3 = 3,
    EM4 = 4
} sleepstate_enum;


void sleep(void);
void blockSleepMode(sleepstate_enum minimumMode);
void unblockSleepMode(sleepstate_enum minimumMode);

#endif
