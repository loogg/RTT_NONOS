#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include "stm32f1xx_std_msp.h"

void uart_init(USART_TypeDef *uart, uint32_t UsartBaudRate, uint16_t UsartWordLength, 
                   uint16_t UsartStopBits, uint16_t UsartParity);
int uart_putc(USART_TypeDef *uart, char c);
int uart_getc(USART_TypeDef *uart, char *ch);
int uart_send_data(USART_TypeDef *uart, uint8_t *buf, uint16_t len);
int uart_read_data(USART_TypeDef *uart, uint8_t *buf, uint16_t len);
#endif
