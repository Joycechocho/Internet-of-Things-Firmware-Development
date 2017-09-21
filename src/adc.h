#define ADC_CLOCK 16000000
#define ADC_CMP_LT_VALUE 4000
#define ADC_CMP_GT_VALUE 0
#define ADC_NUMBER_SAMPLES 200

#define LE_PERIOD_SECONDS (3) // Period must be in the range 0 - 2 seconds
#define LE_ON_SECONDS (.2)    //200mS

void ADC0_setup();
void ADC0_IRQHandler();
void led1_tally();
