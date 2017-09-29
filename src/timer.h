#ifndef _TIMER
#define _TIMER

#include "em_timer.h"
#include "em_cmu.h"

void timer_init(void);
//void TIMER0_setup(void);
void TIMER0_IRQHandler(void);
void Delay_Timer(uint16_t time);

#endif
