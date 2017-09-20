#include <stdint.h>
#include <stdio.h>

#include "em_adc.h"
#include "adc.h"
#include <sleep.h>
#include "main.h"
#include "letimer.h"
#include "gpio.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_core.h"

uint16_t adc_sample_count=0;
uint16_t  adc_sample_buffer[ADC_NUMBER_SAMPLES] = {0};

void ADC0_setup(){

	blockSleepMode(EM3);

	GPIO_PinModeSet(gpioPortA, 0, gpioModeDisabled, 0);

	// Generate configuration structures with default values
	ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	// Configure settings common to single conversion
	init.prescale		= ADC_PrescaleCalc(ADC_CLOCK, CMU_AUXHFRCOBandGet());
	init.timebase 		= ADC_TimebaseCalc(CMU_AUXHFRCOBandGet());
	init.warmUpMode		= adcWarmupKeepADCWarm;
	init.em2ClockConfig = adcEm2ClockOnDemand;

	// Initialize ADC0 with the above settings
	ADC_Init(ADC0, &init);

	// Configure settings for single conversion mode
	singleInit.reference    = adcRefVDD;        // VDD, should be equal to or more than 3.3V
	singleInit.acqTime		= adcAcqTime32;		// Acquisition time			= 32 ADC clock cycles
	singleInit.resolution	= adcRes12Bit;		// Resolution 				= 12 bits
	singleInit.prsEnable 	= true;
	singleInit.diff			= false;			// Differential mode		= false
	singleInit.posSel       = adcPosSelAPORT3XCH8;
	singleInit.prsSel		= adcPRSSELCh0;
	singleInit.fifoOverwrite= true;

	// Initialize ADC0 in single conversion mode with the above settings
	ADC_InitSingle(ADC0, &singleInit);

	//enable compare threshold for single logic
	ADC0->SINGLECTRL |= ADC_SINGLECTRL_CMPEN;
	ADC0->SINGLECTRL |= ADC_SINGLECTRL_REP;

	ADC0->CMPTHR = _ADC_CMPTHR_RESETVALUE;
	ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) +
	    (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);

	// Enable window compare interrupt only
	ADC_IntEnable(ADC0, ADC_IEN_SINGLECMP);

	ADC0->SINGLEFIFOCLEAR = ADC_SINGLEFIFOCLEAR_SINGLEFIFOCLEAR;//clear single conversion fifo

	// Setup ADC interrupts
  	CORE_ATOMIC_IRQ_DISABLE();
  		//ADC0->IFC   = ADC_IFC_SINGLECMP;
	   	//ADC0->IEN   |= ADC_IEN_SINGLECMP;
	 NVIC_ClearPendingIRQ(ADC0_IRQn);
     NVIC_EnableIRQ(ADC0_IRQn);
    CORE_ATOMIC_IRQ_ENABLE();

    ADC0->CMD |= ADC_CMD_SINGLESTART;

}

void ADC0_IRQHandler() {

	int intFlags;
	intFlags = ADC_IntGet(ADC0);
	ADC_IntClear(ADC0, ADC_IFC_SINGLECMP);

	if (intFlags & ADC_IF_SINGLE) {
		   adc_sample_buffer[adc_sample_count] = ADC0->SINGLEDATA;
		   adc_sample_count++;

		   if (adc_sample_count == ADC_NUMBER_SAMPLES) {
			   adc_sample_count = 0;

			    //ADC off
			    ADC0->CMD = ADC_CMD_SINGLESTOP;
			    unblockSleepMode(EM1);

		       led1_tally();
		      }
	}

}
void led1_tally() {
	float average=0;

    for (int i=0; i<ADC_NUMBER_SAMPLES; i++) {
        average+=adc_sample_buffer[i];
    }

    average = average / ADC_NUMBER_SAMPLES;
    average = (average*3.3)/4096;

    if ( average < 2.83 && average > 2.79 ) {
         led1_on();
    } else if ( average < 1.65 && average > 1.62 ) {
         led1_off();
    }
}
