//***********************************************************************************
// Include files
//***********************************************************************************
#include "cmu.h"

//***********************************************************************************
// defined files
//***********************************************************************************


//***********************************************************************************
// global variables
//***********************************************************************************


//***********************************************************************************
// function prototypes
//***********************************************************************************
void cmu_init(void){
	// By default, HFRCO is enabled  cmuHFRCOFreq_19M0Hz
	CMU_HFRCOBandSet(cmuHFRCOFreq_19M0Hz); 				// Set HFRCO frequency
	CMU_OscillatorEnable(cmuOsc_HFXO, false, false);	// Disable HFXO

	// By default, LFRCO is enabled
	CMU_OscillatorEnable(cmuOsc_LFXO, false, false);		// Disable LFXO

	// Peripheral clocks enabled
	CMU_ClockEnable(cmuClock_GPIO, true);
}

