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

void timer_init(void)
{

TIMER_InitCC_TypeDef timer_divider;
TIMER_Init_TypeDef timer_init;

TIMER0->LOCK = _TIMER_LOCK_TIMERLOCKKEY_UNLOCK;

timer_divider.eventCtrl  = timerEventEveryEdge;
timer_divider.edge       = timerEdgeBoth;
timer_divider.prsSel     = timerPRSSELCh0;
timer_divider.cufoa      = timerOutputActionNone;
timer_divider.cofoa      = timerOutputActionNone;
timer_divider.cmoa       = timerOutputActionNone;
timer_divider.mode       = timerCCModeCompare;
timer_divider.filter     = false;
timer_divider.prsInput   = false;
timer_divider.coist      = false;
timer_divider.outInvert  = false;

timer_init.enable = false;
timer_init.debugRun = false;
timer_init.prescale = timerPrescale1024;
timer_init.clkSel = timerClkSelHFPerClk;
timer_init.count2x = false;
timer_init.ati = false;
timer_init.fallAction = timerInputActionNone;
timer_init.riseAction = timerInputActionNone;
timer_init.mode = timerModeUp;
timer_init.dmaClrAct = false;
timer_init.quadModeX4 = false;
timer_init.oneShot = false;
timer_init.sync = false;

TIMER0->CNT = 0x0000;

TIMER_InitCC(TIMER0, 0, &timer_divider);
TIMER_TopSet(TIMER0, 75);
TIMER_IntEnable(TIMER0, TIMER_IF_OF);
NVIC_SetPriority(TIMER0_IRQn, 1);
NVIC_EnableIRQ(TIMER0_IRQn);
TIMER_Init(TIMER0,&timer_init);
}



void TIMER0_setup(void){

	//blockSleepMode(EM1);

	const TIMER_Init_TypeDef timer0Init = {
		    .clkSel = timerClkSelHFPerClk,      /* Select HFPER clock. */
		    .enable = false,                     /* Enable timer when init complete. */
		    .debugRun = false,                  /* Do not stop counter during debug halt. */
		    .fallAction = timerInputActionNone, /* No action on falling input edge. */
		    .riseAction = timerInputActionNone, /* No action on rising input edge. */
		    .mode = timerModeUp,                /* Up-counting. */
		    .dmaClrAct = false,                 /* Do not clear DMA requests when DMA channel is active. */
		    .quadModeX4 = false,                /* Select X2 quadrature decode mode (if used). */
		    .oneShot = false,                   /* one shot. */
		    .sync = false,                       /* Not started/stopped/reloaded by other timers. */
			.count2x = false,
			.ati = false,
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

	CORE_ATOMIC_IRQ_DISABLE();
	TIMER_Enable(TIMER0, false);
	TIMER0->CNT = 0x0000;
	TIMER_IntClear(TIMER0, TIMER_IFC_OF);
	unblockSleepMode(EM1);
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

