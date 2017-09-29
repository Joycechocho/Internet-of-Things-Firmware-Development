#include <stdint.h>
#include <stdio.h>

#include "em_timer.h"
#include "em_adc.h"
#include "em_letimer.h"
#include "adc.h"
#include "sleep.h"
#include "main.h"
#include "letimer.h"
#include "gpio.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_core.h"

void TIMER0_setup(void){

	blockSleepMode(EM1);

	const TIMER_Init_TypeDef timer0Init = {
		    .clkSel = timerClkSelHFPerClk,      /* Select HFPER clock. */
		    .enable = false,                     /* Enable timer when init complete. */
		    .debugRun = true,                  /* Do not stop counter during debug halt. */
		    .fallAction = timerInputActionNone, /* No action on falling input edge. */
		    .riseAction = timerInputActionNone, /* No action on rising input edge. */
		    .mode = timerModeUp,                /* Up-counting. */
		    .dmaClrAct = false,                 /* Do not clear DMA requests when DMA channel is active. */
		    .quadModeX4 = false,                /* Select X2 quadrature decode mode (if used). */
		    .oneShot = true,                   /* one shot. */
		    .sync = false,                       /* Not started/stopped/reloaded by other timers. */
		    .prescale = timerPrescale1024
	};

	TIMER_Init(TIMER0, &timer0Init);

	TIMER_IntEnable(TIMER0, TIMER_IF_OF);    // Enable Timer0 overflow interrupt

	// Setup TIMER interrupts
	CORE_ATOMIC_IRQ_DISABLE();
	    TIMER0->IFC   = TIMER_IFC_OF;
	    TIMER0->IEN   |= TIMER_IFC_OF;
	    NVIC_EnableIRQ(TIMER0_IRQn);
	    NVIC_SetPriority(TIMER0_IRQn, 3); //lower values indicate a higher priority. adc = 4
	CORE_ATOMIC_IRQ_ENABLE();

}

void TIMER0_IRQHandler(void) {

	  unblockSleepMode(EM1);
	  CORE_ATOMIC_IRQ_DISABLE();

	  /* Clear flag for TIMER0 overflow interrupt */
	  TIMER_IntClear(TIMER0, TIMER_IF_OF);
	  CORE_ATOMIC_IRQ_ENABLE();
}



void Delay_Timer(uint16_t time){ //1800ms

	blockSleepMode(EM1);

	TIMER0->CNT = 0;

	/* Start TIMER0 */
	TIMER0->CMD = TIMER_CMD_START;

	/* Wait until counter value is over the threshold */
	while(TIMER0->CNT < time){

	  }

	/* Stop TIMER0 */
	TIMER0->CMD = TIMER_CMD_STOP;



	unblockSleepMode(EM1);
}
