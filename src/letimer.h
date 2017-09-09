#ifndef __LETIMER_H__
#define __LETIMER_H__

#include "sleep.h"


/* Setup LETIMER0 depending on energy mode. Timer
    will interrupt on COMP1, COMP2
    Inputs: e_emode
        Speicify lowest energy mode timer will use
    Globals: none
    Return: none
*/
void LETIMER0_setup(sleepstate_enum);

/* Interrupt routine for LETIMER, flashes LED
    Inputs: none
    Globals: none
    Return: none
*/
void LETIMER0_IRQHandler();

#endif
