#ifndef __UART_CONFIG_H__
#define __UART_CONFIG_H__

#include <rtthread.h>

#ifdef __cplusplus
extern "C" {
#endif


#if defined(BSP_USING_UART1)
#ifndef UART1_CONFIG
#define UART1_CONFIG                                                \
    {                                                               \
        .uart_periph = (uint32_t)USART1,                                         \
    }
#endif /* UART1_CONFIG */
#endif /* BSP_USING_UART1 */

#if defined(BSP_USING_UART2)
#ifndef UART2_CONFIG
#define UART2_CONFIG                                                \
    {                                                               \
        .uart_periph = (uint32_t)USART2,                                         \
    }
#endif /* UART2_CONFIG */
#endif /* BSP_USING_UART2 */


#if defined(BSP_USING_UART3)
#ifndef UART3_CONFIG
#define UART3_CONFIG                                                \
    {                                                               \
        .uart_periph = (uint32_t)USART3,                                         \
    }
#endif /* UART3_CONFIG */
#endif /* BSP_USING_UART3 */


#if defined(BSP_USING_UART4)
#ifndef UART4_CONFIG
#define UART4_CONFIG                                                \
    {                                                               \
        .uart_periph = (uint32_t)UART4,                                          \
    }
#endif /* UART4_CONFIG */
#endif /* BSP_USING_UART4 */


#if defined(BSP_USING_UART5)
#ifndef UART5_CONFIG
#define UART5_CONFIG                                                \
    {                                                               \
        .uart_periph = (uint32_t)UART5,                                          \
    }
#endif /* UART5_CONFIG */
#endif /* BSP_USING_UART5 */

#ifdef __cplusplus
}
#endif

#endif
