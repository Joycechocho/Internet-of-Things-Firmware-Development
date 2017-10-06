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

	TIMER0->LOCK = _TIMER_LOCK_TIMERLOCKKEY_UNLOCK;

	const TIMER_InitCC_TypeDef timer_divider = {
			.eventCtrl  = timerEventEveryEdge,
			.edge       = timerEdgeBoth,
			.prsSel     = timerPRSSELCh0,
			.cufoa      = timerOutputActionNone,
			.cofoa      = timerOutputActionNone,
			.cmoa       = timerOutputActionNone,
			.mode       = timerCCModeCompare,
			.filter     = false,
			.prsInput   = false,
			.coist      = false,
			.outInvert  = false
	};

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

	TIMER0->CNT = 0x0000;

	TIMER_InitCC(TIMER0, 0, &timer_divider);
	TIMER_TopSet(TIMER0, 80);//75
	TIMER_IntEnable(TIMER0, TIMER_IF_OF);
	NVIC_SetPriority(TIMER0_IRQn, 1);
	NVIC_EnableIRQ(TIMER0_IRQn);

	TIMER_Init(TIMER0, &timer0Init);
}


void TIMER0_IRQHandler(void) {

	CORE_ATOMIC_IRQ_DISABLE();
	TIMER_Enable(TIMER0, false);
	TIMER0->CNT = 0x0000;
	TIMER_IntClear(TIMER0, TIMER_IFC_OF);
	unblockSleepMode(EM1);
	CORE_ATOMIC_IRQ_ENABLE();
}

