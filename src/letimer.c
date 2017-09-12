#include "main.h"
#include "letimer.h"
#include "gpio.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_letimer.h"
#include "em_core.h"
#include "sleep.h"


/**************************************************************************//**
 * 	@brief
 * 		Establish the clock tree and set up the LETIMER
 *****************************************************************************/
void LETIMER_setup(sleepstate_enum e){

	// Ticks calculations
	    double le_period_seconds = LE_PERIOD_SECONDS;
	    double le_on_seconds = LE_ON_SECONDS;

	    // LXFO Timings
	    uint16_t le_lfxo_ticks_second = LETIMER_LFXO_TICK_S / (LE_DIVIDER2 ? 2:1); //Divider on/off

	    uint16_t le_comp0_em2 = le_period_seconds * le_lfxo_ticks_second;
	    uint16_t le_comp1_em2 = le_comp0_em2 - (le_on_seconds * le_lfxo_ticks_second);

	    // ULFRCO setup, oscillator ticks are milliseconds
	    //uint16_t le_ulfrco_ticks_second = ulfrco_ticks;
	    uint16_t le_ulfrco_ticks_second = LETIMER_ULFRCO_TICK_S;
	    uint16_t le_comp0_em3 = le_period_seconds * le_ulfrco_ticks_second;
	    uint16_t le_comp1_em3 = le_comp0_em3 - (le_on_seconds * le_ulfrco_ticks_second);


	blockSleepMode(e);

	// LETIMER COMP0 will be used for period, COMP1 for duty cycle time
	if (e < EM3) {
		LETIMER_CompareSet(LETIMER0, 0, le_comp0_em2);
		LETIMER_CompareSet(LETIMER0, 1, le_comp1_em2);
		if (LE_DIVIDER2) { // Scaling needed for periods 2 seconds or greater
		            CMU->LFAPRESC0 |= // prescaler: divide by 2 for timer
		          ((_CMU_LFAPRESC0_LETIMER0_DIV2 << _CMU_LFAPRESC0_LETIMER0_SHIFT) & _CMU_LFAPRESC0_MASK);
		}
		}else {
        LETIMER_CompareSet(LETIMER0, 0, le_comp0_em3);
        LETIMER_CompareSet(LETIMER0, 1, le_comp1_em3);
	}

	// Set configurations for LETIMER 0
	const LETIMER_Init_TypeDef letimerInit =
	{
			  .enable         = false,                   /* Start counting when init completed. */
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
	LETIMER0->CNT=0;


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

	LETIMER_IntClear(LETIMER0,intFlags);

	    if (intFlags & LETIMER_IFS_COMP0) {
	        led0_on();
	    }
	    if (intFlags & LETIMER_IFS_COMP1) {
	        led0_off();
	    }
	CORE_ATOMIC_IRQ_ENABLE();
}
