//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"
#include "sleep.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void cmu_init(sleepstate_enum e){

	// By default, HFRCO is enabled  cmuHFRCOFreq_19M0Hz
	CMU_HFRCOBandSet(cmuHFRCOFreq_19M0Hz); 				// Set HFRCO frequency
	CMU_HFXOAutostartEnable(0, false, false);			//HFXO auto start must be disabled before switching to HFRCO
	CMU_OscillatorEnable(cmuOsc_HFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);	// Disable HFXO to save energy after HFRCO has been enabled

	// By default, LFRCO is enabled
	//CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO

	/* Peripheral clocks enabled */
	//LETIMER0
	 if (e < EM3) {
	        CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
	        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	    } else {
	        CMU_OscillatorEnable(cmuOsc_ULFRCO,true,true);
	        CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
	    }

		CMU_ClockEnable(cmuClock_CORELE, true);
		CMU_ClockEnable(cmuClock_LETIMER0, true);

	//GPIO
	CMU_ClockEnable(cmuClock_GPIO, true);

	//ADC
	/* Select AUXHFRCO for ADC ASYNC mode so that ADC can run on EM2 - EM3 */
	CMU->ADCCTRL = CMU_ADCCTRL_ADC0CLKSEL_AUXHFRCO;

	CMU_AUXHFRCOBandSet(cmuAUXHFRCOFreq_13M0Hz);
	CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
	CMU_ClockSelectSet(cmuClock_AUX,cmuSelect_AUXHFRCO);

	CMU_ClockEnable(cmuClock_ADC0, true);

	//TIMER0
	CMU_ClockEnable(cmuClock_HFPER , true);
	CMU_ClockEnable(cmuClock_TIMER0 , true);

	//USART
	CMU_ClockEnable(cmuClock_USART1 , true);

}


