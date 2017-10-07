//#define LE_PERIOD_SECONDS (3) // Period must be in the range 0 - 2 seconds
//#define LE_ON_SECONDS (0)    //200mS

#define BUTTON_PRESSED_LOWRANGE 30
#define BUTTON_PRESSED_HIGHRANGE 4000

#define BUTTON_RELEASED_LOWRANGE 4000
#define BUTTON_RELEASED_HIGHRANGE 4095

#define SOUTH_THRESHOLD  2200
#define NORTH_THRESHOLD  3800
#define RESET_THRESHOLD  0x34
#define EAST_THRESHOLD  3200
#define WEST_THRESHOLD  2700

void ADC0_setup();
void ADC0_IRQHandler();

extern int set_temperature;
