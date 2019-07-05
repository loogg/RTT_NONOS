#include "rtthread.h"
#include "gd32f2xx_std_msp.h"

void std_init(void)
{
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_PMU);

    /**NOJTAG: JTAG-DP Disabled and SW-DP Enabled 
    */
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);
	
#if defined(RCU_APB2EN_PAEN)
    rcu_periph_clock_enable(RCU_GPIOA);
#endif
    
#if defined(RCU_APB2EN_PBEN)
    rcu_periph_clock_enable(RCU_GPIOB);
#endif
    
#if defined(RCU_APB2EN_PCEN)
    rcu_periph_clock_enable(RCU_GPIOC);
#endif
    
#if defined(RCU_APB2EN_PDEN)
    rcu_periph_clock_enable(RCU_GPIOD);
#endif

#if defined(RCU_APB2EN_PEEN)
    rcu_periph_clock_enable(RCU_GPIOE);
#endif

#if defined(RCU_APB2EN_PFEN)
    rcu_periph_clock_enable(RCU_GPIOF);
#endif

#if defined(RCU_APB2EN_PGEN)
    rcu_periph_clock_enable(RCU_GPIOG);
#endif

}

void uart_msp_init(uint32_t uart)
{
#ifdef BSP_USING_UART0
    if(uart == USART0)
    {
        rcu_periph_clock_enable(RCU_USART0);
        rcu_periph_clock_enable(RCU_GPIOA);

        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    }
#endif
#ifdef BSP_USING_UART1
    if(uart == USART1)
    {
        rcu_periph_clock_enable(RCU_USART1);
        rcu_periph_clock_enable(RCU_GPIOA);

        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
    }
#endif
#ifdef BSP_USING_UART2
    if(uart == USART2)
    {
        rcu_periph_clock_enable(RCU_USART2);
        rcu_periph_clock_enable(RCU_GPIOB);

        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
        gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    }
#endif
#ifdef BSP_USING_UART3
    if(uart == UART3)
    {
        rcu_periph_clock_enable(RCU_UART3);
        rcu_periph_clock_enable(RCU_GPIOC);

        gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
        gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
    }
#endif
#ifdef BSP_USING_UART4
    if(uart == UART4)
    {
        rcu_periph_clock_enable(RCU_UART4);
        rcu_periph_clock_enable(RCU_GPIOC);
        rcu_periph_clock_enable(RCU_GPIOD);

        gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
        gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    }
#endif
#ifdef BSP_USING_UART5
    if(uart == USART5)
    {
        rcu_periph_clock_enable(RCU_USART5);
        rcu_periph_clock_enable(RCU_GPIOC);

        gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
        gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_7);
    }
#endif
}

void spi_msp_init(uint32_t spi_periph)
{
#ifdef BSP_USING_SPI0
    if(spi_periph == SPI0)
    {
        rcu_periph_clock_enable(RCU_SPI0);
        rcu_periph_clock_enable(RCU_GPIOA);
        gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
        gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6); 
    }
#endif
#ifdef BSP_USING_SPI1
    if(spi_periph == SPI1)
    {
        rcu_periph_clock_enable(RCU_SPI1);
        rcu_periph_clock_enable(RCU_GPIOB);
        gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
        gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    }
#endif
#ifdef BSP_USING_SPI2
    if(spi_periph == SPI2)
    {
        rcu_periph_clock_enable(RCU_SPI2);
        rcu_periph_clock_enable(RCU_GPIOC);
        gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10 | GPIO_PIN_12);
        gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
        gpio_pin_remap_config(GPIO_SPI2_REMAP, ENABLE);
    }
#endif
}

