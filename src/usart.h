void USART1_setup();
void bma_wakeup(void);
uint8_t bma280_read_byte(USART_TypeDef *usart, uint8_t reg_addr);
uint8_t bma280_write_byte(USART_TypeDef *usart, uint8_t reg_addr, uint8_t data);
void normal_to_suspend(void);
void suspend_to_normal(void);
void BMA280_enable(void);
void BMA280_disable(void);
void GPIO_ODD_IRQHandler(void);
