#ifndef __STM32F1XX_STD_MSP_H
#define __STM32F1XX_STD_MSP_H
#include "stm32f10x.h"

void std_init(void);
void uart_msp_init(USART_TypeDef *uart);
#endif
