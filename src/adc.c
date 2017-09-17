#include <stdint.h>

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


void ADC0_setup(){

	blockSleepMode(EM3);

	GPIO_PinModeSet(gpioPortA, 0, gpioModeDisabled, 0);

	// Generate configuration structures with default values
	ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	// Configure settings common to both single conversion and scan mode (see em_adc.h)
	init.prescale		= ADC_PrescaleCalc(9000000, 0);	// ADC_clock = 9MHz
	init.ovsRateSel 	= 1;
	init.timebase 		= ADC_TimebaseCalc(0);
	init.warmUpMode		= adcWarmupKeepADCWarm;

	// Initialize ADC0 with the above settings
	ADC_Init(ADC0, &init);

	// Configure settings for single conversion mode
	singleInit.reference  = adcRef2V5;
	singleInit.acqTime		= adcAcqTime32;		// Acquisition time			= 32 ADC clock cycles
	singleInit.resolution	= adcRes12Bit;		// Resolution 				= 12 bits
	singleInit.rep			= true;
	singleInit.diff			= false;			// Differential mode		= false

	// Initialize ADC0 in single conversion mode with the above settings
	ADC_InitSingle(ADC0, &singleInit);


	// Setup ADC interrupts
  	CORE_ATOMIC_IRQ_DISABLE();
  		ADC0->IFC   = ADC_IFC_SINGLECMP;
	   	ADC0->IEN   |= ADC_IEN_SINGLECMP;
     NVIC_EnableIRQ(ADC0_IRQn);
    CORE_ATOMIC_IRQ_ENABLE();

}

void ADC0_IRQHandler() {
	int intFlags;
	float  sampleValue;

    float north = 2.83; //V
    float south = 1.65;
	float east = 2.53;
	float west = 1.98;

	CORE_ATOMIC_IRQ_DISABLE();
	intFlags = ADC_IntGet(ADC0);
	ADC_IntClear(ADC0,intFlags);

	if (intFlags & ADC_IF_SINGLECMP) {

		ADC0->CMD = ADC_CMD_SINGLESTOP;

		sampleValue = ADC_DataSingleGet(ADC0);
		double voltage = (sampleValue * 2500) / 4096;

		 if ( voltage == north ) {
			         led1_on();
			    }else if (voltage == south){
			         led1_off();
			    }else if (voltage == east){
			         //increment on time of LED0 by 500mS
			    }else if (voltage == west){
			        //decrement on time of LED0 by 500mS
			    }

	}
	CORE_ATOMIC_IRQ_ENABLE();
}

