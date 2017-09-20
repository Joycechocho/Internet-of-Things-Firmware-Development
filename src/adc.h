#define ADC_CLOCK 16000000
#define ADC_CMP_LT_VALUE 4000
#define ADC_CMP_GT_VALUE 0
#define ADC_NUMBER_SAMPLES 200

void ADC0_setup();
void ADC0_IRQHandler();
void led1_tally();
