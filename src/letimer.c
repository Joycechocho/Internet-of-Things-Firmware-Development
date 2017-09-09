#include "letimer.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_letimer.h"
#include "em_core.h"


/**************************************************************************//**
 * 	@brief
 * 		Establish the clock tree and set up the LETIMER
 *****************************************************************************/
void LETIMER_setup(sleepstate_enum e){

	blockSleepMode(e);

    if (e < EM3) {
        CMU_OscillatorEnable(cmuOsc_LFXO, true, false);
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
    } else {
        CMU_OscillatorEnable(cmuOsc_ULFRCO,true,false);
        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
    }

	CMU_ClockEnable(cmuClock_CORELE, true);
	CMU_ClockEnable(cmuClock_LETIMER0, true);

	// LETIMER COMP0 will be used for period, COMP1 for duration
	if (e < EM3) {
	    LETIMER_CompareSet(LETIMER0, 0, LETIMER_TOP_EM2);
	    LETIMER_CompareSet(LETIMER0, 1, LETIMER_COMP1_EM2);
	} else {
	    LETIMER_CompareSet(LETIMER0, 0, LETIMER_TOP_EM3);
	    LETIMER_CompareSet(LETIMER0, 1, LETIMER_COMP1_EM3);
	}

	// Set configurations for LETIMER 0
	const LETIMER_Init_TypeDef letimerInit =
	{
			  .enable         = true,                   /* Start counting when init completed. */
			  .debugRun       = false,                  /* Counter shall not keep running during debug halt. */
			  .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
			  .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
			  .out0Pol        = 0,                      /* Idle value for output 0. */
			  .out1Pol        = 0,                      /* Idle value for output 1. */
			  .ufoa0          = letimerUFOAPwm,         /* PWM output on output 0 */
			  .ufoa1          = letimerUFOAPulse,       /* Pulse output on output 1*/
			  .repMode        = letimerRepeatFree       /* Count until stopped */
	};

	LETIMER_Init(LETIMER0, &letimerInit);

    // Setup LETIMER interrupts
	CORE_ATOMIC_IRQ_DISABLE();
    LETIMER0->IFC   = LETIMER_IFC_COMP0;
    LETIMER0->IFC   = LETIMER_IFC_COMP1;
    LETIMER0->IEN   |= LETIMER_IEN_COMP0;
    LETIMER0->IEN   |= LETIMER_IEN_COMP1;
    NVIC_EnableIRQ(LETIMER0_IRQn);
	CORE_ATOMIC_IRQ_ENABLE();

    LETIMER_Enable(LETIMER0, true);
}


/**************************************************************************//**
 * 	@brief
 * 		LETimer Interrupt Handler.
 *****************************************************************************/
void LETIMER0_IRQHandler(void) {
	int intFlags;
	CORE_ATOMIC_IRQ_DISABLE();
	intFlags = LETIMER_IntGet(LETIMER0);
	    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP0);
	    LETIMER_IntClear(LETIMER0,LETIMER_IFS_COMP1);

	    if (intFlags & LETIMER_IFS_COMP0) {
	        led0_on();
	    }
	    if (intFlags & LETIMER_IFS_COMP1) {
	        led0_off();
	    }
	CORE_ATOMIC_IRQ_ENABLE();
}
