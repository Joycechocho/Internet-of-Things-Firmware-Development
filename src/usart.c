#include <stdint.h>
#include <stdio.h>

#include "em_usart.h"
#include "em_timer.h"
#include "em_adc.h"
#include "em_letimer.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_int.h"
#include "em_core.h"
#include "em_gpio.h"

#include "adc.h"
#include "sleep.h"
#include "main.h"
#include "letimer.h"
#include "gpio.h"
#include "timer.h"

volatile int gpio_flag =0;

void USART1_setup(){

	USART_TypeDef *spi = USART1;

	const USART_InitSync_TypeDef spi_init = {
	        .enable = usartDisable,
	        .refFreq = 0,
	        .baudrate = 50000,
	        .databits = usartDatabits8,
	        .master = true,
	        .msbf = true,
	        .clockMode = usartClockMode3, //rising edge
	        .autoTx = false,
	        .autoCsEnable = true,
	        .autoCsHold = 0,
	        .autoCsSetup = 0,
			.prsRxEnable = 0,
			.prsRxCh = usartPrsRxCh0,
	};

	USART_InitSync(spi, &spi_init);

    /* Set GPIO config to master */
	GPIO_PinModeSet(gpioPortC, 6, gpioModePushPull, 1); //mosi
	GPIO_PinModeSet(gpioPortC, 7, gpioModeInput, 0); //miso
	GPIO_PinModeSet(gpioPortC, 8, gpioModePushPull, 1); //sclk
	GPIO_PinModeSet(gpioPortC, 9, gpioModePushPull, 1); //cs

	/* Enabling pins */
	spi->ROUTELOC0 = USART_ROUTELOC0_CLKLOC_LOC11 | USART_ROUTELOC0_TXLOC_LOC11 | USART_ROUTELOC0_RXLOC_LOC11 | USART_ROUTELOC0_CSLOC_LOC11;
	spi->ROUTEPEN = USART_ROUTEPEN_CLKPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_CSPEN;

	/* Clearing transmit/receive buffer */
	spi->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

	USART_Enable(USART1, usartEnable);

	//normal_to_suspend();
	//Delay_Timer(1800);
}

/*
uint8_t bma280_read_byte(USART_TypeDef *usart, uint8_t address)
{
    uint16_t data;
    uint32_t tmp;

    data = 0x0080; // note: byte order when sending is 0x80 0x00 . MSB is set to indicate read for BMA280 chip
    data = data | address;  // set address field

    while (!(usart->STATUS & USART_STATUS_TXBL));
    usart->TXDOUBLE = (uint32_t)data;
    while (!(usart->STATUS & USART_STATUS_TXC));
    tmp = usart->RXDOUBLE;
    tmp = tmp >> 8;
    return (uint8_t)tmp;
}
*/

void bma_wakeup(void)
{
	__enable_irq();
	TIMER_Enable(TIMER0,true);
	blockSleepMode(EM1);
}

uint8_t bma280_read_byte(USART_TypeDef *usart, uint8_t reg_addr){
	uint16_t final_value = 0x0000;
	uint8_t result;


	final_value |= 0xff;
	final_value <<=8;
	final_value |= (reg_addr | 0x80);

	while (!(USART1->STATUS & USART_STATUS_TXBL));
	USART1 -> TXDOUBLE = final_value;
	while (!(USART1->STATUS & USART_STATUS_TXC));

	result= (USART1 -> RXDOUBLEXP & _USART_RXDOUBLEXP_RXDATAP1_MASK );
	USART1->CMD |= _USART_CMD_CLEARRX_MASK;
	USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);
	return result;
}
/*
void bma280_write_byte(USART_TypeDef *usart, uint8_t addr, uint8_t data ){

	uint16_t tmp;
	uint16_t total;
    total = (uint16_t)(data << 8 ) | (uint16_t) addr ;

    while (!(usart->STATUS & USART_STATUS_TXBL)) ;//Check that transmit buffer is empty
    usart->TXDOUBLE = total; //16bits

    while (!(usart->STATUS & USART_STATUS_TXC)) ;//Waiting for transmission of last byte
    tmp = usart->RXDOUBLE; //reading out the data
}
*/

uint8_t bma280_write_byte(USART_TypeDef *usart, uint8_t reg_addr, uint8_t data){
	uint16_t final_value = 0x0000;
	uint8_t result;


	final_value |= data;
	final_value <<=8;
	final_value |= reg_addr;

	while (!(USART1->STATUS & USART_STATUS_TXBL));
	USART1 -> TXDOUBLE = final_value;
	while (!(USART1->STATUS & USART_STATUS_TXC));

	result= (USART1 -> RXDOUBLEXP & _USART_RXDOUBLEXP_RXDATAP1_MASK );
	USART1->CMD |= _USART_CMD_CLEARRX_MASK;
	USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);
	return result;
}

void normal_to_suspend(void){
	//uint8_t data = bma280_read_byte(USART1, 0x11);
	//uint8_t txdata = 0x80 | data;
	//bma280_write_byte(USART1, 0x11, txdata );
	bma280_write_byte(USART1, 0x11, 0x80);
}

void suspend_to_normal(void){
	//uint8_t data = bma280_read_byte(USART1, 0x11);
	//uint8_t txdata = 0x7F & data;
	//bma280_write_byte(USART1, 0x11, txdata );
	bma280_write_byte(USART1, 0x11, 0x00);
	bma_wakeup();
}

void BMA280_enable(void){

	suspend_to_normal();
	//Delay_Timer(1800);

	/* Range +/- 4g */
	bma280_write_byte(USART1, 0x0F, 0x05);

	/* Bandwidth 125Hz */
	bma280_write_byte(USART1, 0x10, 0x0C);

	/* Tap quiet 30mS */
	/*Tap duration 200mS */
	/*Tap shock 50mS*/
	bma280_write_byte(USART1, 0x2A, 0x03);

	/* Tap samples 4 */
	/*Tap threshold 250mg*/
	bma280_write_byte(USART1, 0x2B, 0x48);
	bma280_write_byte(USART1, 0x16, 0x30);
	bma280_write_byte(USART1, 0x19, 0x30);


	//NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);//clear any pending interrupts;
	//NVIC_DisableIRQ(GPIO_ODD_IRQn);


	GPIO_PinModeSet(gpioPortD, 11, gpioModeInput, 0);
	GPIO_IntConfig(gpioPortD, 11, true, false, true);//Set BG GPIO pin as Input to interrupt BG on BMA280 interrupt on rising edge;

	NVIC_EnableIRQ(GPIO_ODD_IRQn);	//Enable BG BMA280 GPIO interrupt pin ;
}

void BMA280_disable(void){
	normal_to_suspend();
	//Delay_Timer(1800);
}







void GPIO_ODD_IRQHandler(void){
/*
  //GPIO->IFC = 0x00000800;
  if(bma280_read_byte(USART1, 0x09) == 0x16){//single tap
	  led1_off();
	  //bma280_write_byte(USART1, 0x16, 0x10);

  }else if(bma280_read_byte(USART1, 0x09) == 0x08){//double tap
	  led1_on();
	  //bma280_write_byte(USART1, 0x16, 0x10);

  }
  */

	  __disable_irq();

		GPIO->IFC = 0x00000800;

		if(gpio_flag==0){
			//GPIO_PinOutClear(LED1_port,LED1_pin);
			led1_off();

			while (!(USART1->STATUS & USART_STATUS_TXBL));
			USART1->TXDOUBLE= 0x1016;
			while (!(USART1->STATUS & USART_STATUS_TXC));
			USART1->CMD |= _USART_CMD_CLEARRX_MASK;
			USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);

			while (!(USART1->STATUS & USART_STATUS_TXBL));
			USART1->TXDOUBLE= 0x1019;
			while (!(USART1->STATUS & USART_STATUS_TXC));
			USART1->CMD |= _USART_CMD_CLEARRX_MASK;
			USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);
			gpio_flag =1;
	     }
	     else{
	    	//GPIO_PinOutSet(LED1_port,LED1_pin);
	    	 led1_on();
	    	while (!(USART1->STATUS & USART_STATUS_TXBL));
	    	USART1->TXDOUBLE= 0x2016;
	    	while (!(USART1->STATUS & USART_STATUS_TXC));
	    	USART1->CMD |= _USART_CMD_CLEARRX_MASK;
	    	USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);

	  	 	while (!(USART1->STATUS & USART_STATUS_TXBL));
	   	 	USART1->TXDOUBLE= 0x2019;
	   	 	while (!(USART1->STATUS & USART_STATUS_TXC));
	   	 	USART1->CMD |= _USART_CMD_CLEARRX_MASK;
	   	 	USART1->CMD &= ~(_USART_CMD_CLEARRX_MASK);
	   	 	gpio_flag = 0;
	     }
		__enable_irq();



}

