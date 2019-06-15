#include "drv_usart.h"

void uart_init(USART_TypeDef *uart, uint32_t UsartBaudRate, uint16_t UsartWordLength, 
                   uint16_t UsartStopBits, uint16_t UsartParity)
{
    USART_InitTypeDef USART_InitStructure;
    uart_msp_init(uart);
    USART_InitStructure.USART_BaudRate = UsartBaudRate;
    USART_InitStructure.USART_WordLength = UsartWordLength;
    USART_InitStructure.USART_StopBits = UsartStopBits;
    USART_InitStructure.USART_Parity = UsartParity;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(uart, &USART_InitStructure);
    USART_Cmd(uart, ENABLE);
    USART_ClearFlag(uart, USART_FLAG_TC);
}

int uart_putc(USART_TypeDef *uart, char c)
{
    USART_ClearFlag(uart, USART_FLAG_TC);
    uart->DR = c;
    while(USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET);
    return 1;
}

int uart_getc(USART_TypeDef *uart, char *ch)
{
    if(USART_GetFlagStatus(uart, USART_FLAG_RXNE) != RESET)
    {
        *ch = uart->DR & 0xff;
        return 1;
    }
    return 0;
}

