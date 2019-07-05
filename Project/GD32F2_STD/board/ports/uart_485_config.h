#ifndef __UART_485_CONFIG_H__
#define __UART_485_CONFIG_H__
#include <rtthread.h>


#if defined(BSP_UART2_485)
#ifndef UART2_485_CONFIG
#define UART2_485_CONFIG                          \
    {                                             \
		.gpio_periph = GPIOB,							  \
        .pin = GPIO_PIN_12,                        \
        .send_logical = 0,                  \
    }
#endif /* UART2_485_CONFIG */
#endif /* BSP_UART2_485 */

#endif
