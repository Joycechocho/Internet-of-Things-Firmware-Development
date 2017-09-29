void USART1_setup();
uint8_t bma280_read_byte(USART_TypeDef *usart, uint8_t offset);
void bma280_write_byte(USART_TypeDef *usart, uint8_t addr, uint8_t data );
void BMA280_enable(void);
void BMA280_disable(void);
void suspend_to_normal(void);
void normal_to_suspend(void);
void bma_wakeup(void);
