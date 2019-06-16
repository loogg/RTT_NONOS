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
    return uart_send_data(uart, (uint8_t *)&c, 1);
}

int uart_getc(USART_TypeDef *uart, char *ch)
{
    return uart_read_data(uart, (uint8_t *)ch, 1);
}

int uart_send_data(USART_TypeDef *uart, uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;
    while(i < len)
    {
        USART_ClearFlag(uart, USART_FLAG_TC);
        uart->DR = *buf++;
        while(USART_GetFlagStatus(uart, USART_FLAG_TC) == RESET);
        i++;
    }
    return len;
}

int uart_read_data(USART_TypeDef *uart, uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;
    while(i < len)
    {
        if(USART_GetFlagStatus(uart, USART_FLAG_RXNE) != RESET)
        {
            *buf++ = uart->DR & 0xff;
            i++;
        }
        else
            break;
    }
    return i;
}

