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


    ADC0->CMPTHR = (BUTTON_PRESSED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_PRESSED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);

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

    static bool switch_released;
	uint32_t adc_value;

	// Ticks calculations
	double le_period_seconds = LE_PERIOD_SECONDS;
	double le_on_seconds = LE_ON_SECONDS;

	uint16_t le_lfxo_ticks_second = LETIMER_LFXO_TICK_S / (LE_DIVIDER2 ? 2:1); //Divider on/off
    uint16_t le_comp0_em2 = le_period_seconds * le_lfxo_ticks_second;
    uint16_t le_comp1_em2 = le_comp0_em2 - (le_on_seconds * le_lfxo_ticks_second);

	int intFlags;
	CORE_ATOMIC_IRQ_DISABLE();

	intFlags = ADC_IntGet(ADC0);

	adc_value = (ADC0->SINGLEDATA);

	/******************************************************************************
	The IF function below, credited to Sudeep Kulkarni
	*******************************************************************************/
	    if(switch_released){
	        if(adc_value < RESET_THRESHOLD){									//button press to reset
	            LETIMER_CompareSet(LETIMER0,1,le_comp1_em2);
	            led1_off();												//turning the LED of as required by the assignment of reset
	            switch_released = false;
	            ADC0->CMPTHR = (BUTTON_RELEASED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_RELEASED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);
	        }
	        else if (adc_value < SOUTH_THRESHOLD){							//south turning off the led 1
	        	led1_off();													//turning the led off as per the requirements
	            switch_released = false;
	            ADC0->CMPTHR = (BUTTON_RELEASED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_RELEASED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);
	        }
	        else if(adc_value < WEST_THRESHOLD){										//this is west hence decrement
	            onTime = ((float)LETIMER_CompareGet(LETIMER0, 1)/CurrentLFAFreq);
	           // if (onTime >= 0.5){
	                onTime = (onTime - 0.5)*CurrentLFAFreq;
	                LETIMER_CompareSet(LETIMER0,1,onTime);
	               switch_released = false;
	                ADC0->CMPTHR = (BUTTON_RELEASED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_RELEASED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);
	           // }
	        }
	        else if (adc_value < EAST_THRESHOLD){									//this is east hence increment
	            onTime = ((float)LETIMER_CompareGet(LETIMER0, 1)/CurrentLFAFreq);
	           // if(onTime < 3 - 0.5){
	                onTime = (onTime + 0.5)*CurrentLFAFreq;							//new on time period set
	                LETIMER_CompareSet(LETIMER0,1,onTime);
	                switch_released = false;										//setting switch released to false
	                ADC0->CMPTHR = (BUTTON_RELEASED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_RELEASED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);	//setting the value from 4000 to 4096 for switch release
	           // }
	        }
	        else if(adc_value< NORTH_THRESHOLD){											//north turning on the led 1
	            led1_on();
	            switch_released = false;
	            ADC0->CMPTHR = (BUTTON_RELEASED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_RELEASED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT); //setting the value from 4000 to 4096 for switch release
	        }
	    }
	    else if(adc_value > NORTH_THRESHOLD){											//setting the switch release to true when the value is greater than the greatest
	        switch_released = true;
	        ADC0->CMPTHR = (BUTTON_PRESSED_HIGHRANGE<<_ADC_CMPTHR_ADLT_SHIFT)|(BUTTON_PRESSED_LOWRANGE<<_ADC_CMPTHR_ADGT_SHIFT);
	    }


    ADC0->SINGLEFIFOCLEAR = 1;
    ADC_IntClear(ADC0,intFlags);

    CORE_ATOMIC_IRQ_ENABLE();

}
