#ifndef __DRV_USART_H__
#define __DRV_USART_H__

#include "stm32f1xx_std_msp.h"
#include "rtthread.h"

struct uart_485_config
{
    uint32_t gpio_periph;
	uint32_t pin;
    uint32_t send_logical;
};

/* stm32 uart dirver class */
struct stm32_uart
{
    uint32_t uart_periph;
	struct uart_485_config *rs485;
    uint8_t uart_485_flag;
};

void uart_pre_init(void);
void uart_init(uint32_t uart, uint32_t UsartBaudRate, uint16_t UsartWordLength, 
                   uint16_t UsartStopBits, uint16_t UsartParity);
int uart_putc(uint32_t uart, char c);
int uart_getc(uint32_t uart, char *ch);
int uart_send_data(uint32_t uart, uint8_t *buf, uint16_t len);
int uart_read_data(uint32_t uart, uint8_t *buf, uint16_t len);
#endif
