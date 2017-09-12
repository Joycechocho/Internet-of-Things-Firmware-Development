#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"

extern uint16_t ulfrco_ticks;

#define LETIMER_ULFRCO_TICK_S  1000
#define LETIMER_LFXO_TICK_S   32768

#define LE_DIVIDER2 true        //Needed for periods 2 seconds and over
#define LE_PERIOD_SECONDS (2.5) // Period must be in the range 0 - 2 seconds
#define LE_ON_SECONDS (.05)    //  Off + On period duration must cumulatively be 2 seconds or less





/* Setup LETIMER0 depending on energy mode. Timer
    will interrupt on COMP1, COMP2
    Inputs: e_emode
        Speicify lowest energy mode timer will use
    Globals: none
    Return: none
*/
void LETIMER_setup(sleepstate_enum);

/* Interrupt routine for LETIMER, flashes LED
    Inputs: none
    Globals: none
    Return: none
*/
void LETIMER0_IRQHandler(void);

#endif
