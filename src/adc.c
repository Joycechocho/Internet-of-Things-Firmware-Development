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

void ADC0_setup(){

	blockSleepMode(EM2);

	GPIO_PinModeSet(gpioPortA, 0, gpioModeDisabled, 0);

	// Generate configuration structures with default values
	ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

	// Configure settings common to single conversion
	init.prescale		= CMU_ClockFreqGet(cmuClock_AUX)/(200 + 1);
	init.warmUpMode		= adcWarmupNormal;
	init.em2ClockConfig = adcEm2ClockAlwaysOn;
	init.timebase = ADC_TimebaseCalc(0);
    init.tailgate = false;

	// Initialize ADC0 with the above settings
	ADC_Init(ADC0, &init);

	// Configure settings for single conversion mode
	singleInit.reference    = adcRefVDD;        // VDD, should be equal to or more than 3.3V
	singleInit.acqTime		= adcAcqTime32;		// Acquisition time			= 32 ADC clock cycles
	singleInit.resolution	= adcRes12Bit;		// Resolution 				= 12 bits
	singleInit.rep = true;
	singleInit.posSel       = adcPosSelAPORT3XCH8;
	singleInit.fifoOverwrite= true;

	// Initialize ADC0 in single conversion mode with the above settings
	ADC_InitSingle(ADC0, &singleInit);

	//enable compare threshold for single logic
	ADC0->SINGLECTRL |= ADC_SINGLECTRL_CMPEN;

	ADC0->CMPTHR = (ADC_CMP_GT_VALUE << _ADC_CMPTHR_ADGT_SHIFT) |
	    (ADC_CMP_LT_VALUE << _ADC_CMPTHR_ADLT_SHIFT);

	 ADC0->BIASPROG = ADC_BIASPROG_ADCBIASPROG_NORMAL;
	 ADC0->BIASPROG = ADC_BIASPROG_GPBIASACC_LOWACC;

	// Enable window compare interrupt only
	ADC_IntEnable(ADC0, ADC_IEN_SINGLECMP);

	// Setup ADC interrupts
  	CORE_ATOMIC_IRQ_DISABLE();
  		ADC0->IFC   = ADC_IFC_SINGLECMP;
	   	ADC0->IEN   |= ADC_IEN_SINGLECMP;
	 NVIC_ClearPendingIRQ(ADC0_IRQn);
     NVIC_EnableIRQ(ADC0_IRQn);
    CORE_ATOMIC_IRQ_ENABLE();

    ADC0->CMD |= ADC_CMD_SINGLESTART;

}

void ADC0_IRQHandler() {
    float static onTime;
    uint32_t CurrentLFAFreq = CMU_ClockFreqGet(cmuClock_LFA);

	uint32_t adc_value;

	int intFlags;
	CORE_ATOMIC_IRQ_DISABLE();

	intFlags = ADC_IntGet(ADC0);
    //ADC_IntClear(ADC0, ADC_IFC_SINGLECMP);

	    unblockSleepMode(EM1);
	    adc_value = (ADC0->SINGLEDATA);

	        if (adc_value < 0xDBA && adc_value > 0xDB1){							//north
			       led1_on();
	            ADC0->CMPTHR = (0x1000<<_ADC_CMPTHR_ADLT_SHIFT)|(0xFA0<<_ADC_CMPTHR_ADGT_SHIFT);
	        }
	        else if(adc_value < 0x999 && adc_value > 0x996){		//west
	            onTime = ((float)LETIMER_CompareGet(LETIMER0, 1)/CurrentLFAFreq);
	                onTime = (onTime - 0.5)*CurrentLFAFreq;
	                LETIMER_CompareSet(LETIMER0,1,onTime);
	                ADC0->CMPTHR = (0x1000<<_ADC_CMPTHR_ADLT_SHIFT)|(0xFA0<<_ADC_CMPTHR_ADGT_SHIFT);
	        }
	        else if (adc_value < 0xC43 && adc_value > 0xC40){								//east
	            onTime = ((float)LETIMER_CompareGet(LETIMER0, 1)/CurrentLFAFreq);
	                onTime = (onTime + 0.5)*CurrentLFAFreq;
	                LETIMER_CompareSet(LETIMER0,1,onTime);
	                ADC0->CMPTHR = (0x1000<<_ADC_CMPTHR_ADLT_SHIFT)|(0xFA0<<_ADC_CMPTHR_ADGT_SHIFT);
	            //}
	        }
	        else if(adc_value < 0x804 && adc_value > 0x7FA){											//south
			       led1_off();
	            ADC0->CMPTHR = (0x1000<<_ADC_CMPTHR_ADLT_SHIFT)|(0xFA0<<_ADC_CMPTHR_ADGT_SHIFT);
	        }


    ADC0->SINGLEFIFOCLEAR = 1;
    ADC_IntClear(ADC0,intFlags);

    CORE_ATOMIC_IRQ_ENABLE();

}
