#include "drv_usart.h"
#include "drv_config.h"

#ifdef BSP_USING_UART_485
#include "uart_485_config.h"
#endif

enum
{
#ifdef BSP_USING_UART0
    UART0_INDEX,
#endif
#ifdef BSP_USING_UART1
    UART1_INDEX,
#endif
#ifdef BSP_USING_UART2
    UART2_INDEX,
#endif
#ifdef BSP_USING_UART3
    UART3_INDEX,
#endif
#ifdef BSP_USING_UART4
    UART4_INDEX,
#endif
#ifdef BSP_USING_UART5
    UART5_INDEX,
#endif
};



static struct gd32_uart uart_obj[] =
{
#ifdef BSP_USING_UART0
        UART0_CONFIG,
#endif
#ifdef BSP_USING_UART1
        UART1_CONFIG,
#endif
#ifdef BSP_USING_UART2
        UART2_CONFIG,
#endif
#ifdef BSP_USING_UART3
        UART3_CONFIG,
#endif
#ifdef BSP_USING_UART4
        UART4_CONFIG,
#endif
#ifdef BSP_USING_UART5
        UART5_CONFIG,
#endif
};

static void gd32_uart_get_485_config(void)
{
#ifdef BSP_UART0_485
    uart_obj[UART0_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart0_485 = UART0_485_CONFIG;
    uart_obj[UART0_INDEX].rs485 = &uart0_485;
#endif
#ifdef BSP_UART1_485
    uart_obj[UART1_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart1_485 = UART1_485_CONFIG;
    uart_obj[UART1_INDEX].rs485 = &uart1_485;
#endif
#ifdef BSP_UART2_485
    uart_obj[UART2_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart2_485 = UART2_485_CONFIG;
    uart_obj[UART2_INDEX].rs485 = &uart2_485;
#endif
#ifdef BSP_UART3_485
    uart_obj[UART3_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart3_485 = UART3_485_CONFIG;
    uart_obj[UART3_INDEX].rs485 = &uart3_485;
#endif
#ifdef BSP_UART4_485
    uart_obj[UART4_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart4_485 = UART4_485_CONFIG;
    uart_obj[UART4_INDEX].rs485 = &uart4_485;
#endif
#ifdef BSP_UART5_485
    uart_obj[UART5_INDEX].uart_485_flag = 1;
    static struct uart_485_config uart5_485 = UART5_485_CONFIG;
    uart_obj[UART5_INDEX].rs485 = &uart5_485;
#endif
}

void uart_pre_init(void)
{
	gd32_uart_get_485_config();
}

static struct gd32_uart *uart_get_elm(uint32_t uart)
{
	struct gd32_uart *elm = RT_NULL;
	uint8_t obj_num = sizeof(uart_obj) / sizeof(struct gd32_uart);
	for(uint8_t i=0;i<obj_num;i++)
	{
		if(uart_obj[i].uart_periph == uart)
		{
			elm = &uart_obj[i];
		}
	}
	return elm;
}

static void uart_set_control_pin(struct uart_485_config *rs485, uint8_t on)
{
	gpio_bit_write(rs485->gpio_periph, rs485->pin, on);
}

static void uart_control_pin_init(uint32_t uart)
{
	struct gd32_uart *elm = uart_get_elm(uart);
	if(elm->uart_485_flag)
	{
		gpio_init(elm->rs485->gpio_periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, elm->rs485->pin);
		uart_set_control_pin(elm->rs485, !elm->rs485->send_logical);
	}
}


void uart_init(uint32_t uart, uint32_t UsartBaudRate, uint16_t UsartWordLength, 
                   uint16_t UsartStopBits, uint16_t UsartParity)
{
    uart_msp_init(uart);
	uart_control_pin_init(uart);
    usart_disable(uart);
    usart_deinit(uart);
    usart_baudrate_set(uart, UsartBaudRate);
    usart_word_length_set(uart, UsartWordLength);
    usart_stop_bit_set(uart, UsartStopBits);
    usart_parity_config(uart, UsartParity);
    usart_receive_config(uart, USART_RECEIVE_ENABLE);
    usart_transmit_config(uart, USART_TRANSMIT_ENABLE);
    usart_hardware_flow_rts_config(uart, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(uart, USART_CTS_DISABLE);
    usart_enable(uart);
    usart_flag_clear(uart, USART_FLAG_TC);
}

int uart_putc(uint32_t uart, char c)
{
    return uart_send_data(uart, (uint8_t *)&c, 1);
}

int uart_getc(uint32_t uart, char *ch)
{
    return uart_read_data(uart, (uint8_t *)ch, 1);
}

int uart_send_data(uint32_t uart, uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;
	struct gd32_uart *elm = uart_get_elm(uart);
	if(elm->uart_485_flag)
	{
		uart_set_control_pin(elm->rs485, elm->rs485->send_logical);
	}
    while(i < len)
    {
        usart_flag_clear(uart, USART_FLAG_TC);
        usart_data_transmit(uart, *buf++);
        uint32_t time_out = 0;
        while(usart_flag_get(uart, USART_FLAG_TC) == RESET)
		{
            if(++time_out > 100000)
            {
                goto _exit;
            }
		}
        i++;
    }

_exit:
	if(elm->uart_485_flag)
	{
		uart_set_control_pin(elm->rs485, !elm->rs485->send_logical);
	}
    return i;
}

int uart_read_data(uint32_t uart, uint8_t *buf, uint16_t len)
{
    uint16_t i = 0;
    while(i < len)
    {
        if(usart_flag_get(uart, USART_FLAG_RBNE) != RESET)
        {
            *buf++ = usart_data_receive(uart);
            i++;
        }
        else
            break;
    }
    return i;
}

