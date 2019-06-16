#include "board.h"
#include "drv_common.h"

#ifdef RT_USING_FINSH
#include <finsh.h>
static void reboot(uint8_t argc, char **argv)
{
    rt_hw_cpu_reset();
}
FINSH_FUNCTION_EXPORT_ALIAS(reboot, __cmd_reboot, Reboot System);
#endif /* RT_USING_FINSH */

/* SysTick configuration */
void rt_system_tick_init(void)
{
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    rt_tick_increase();
}



/**
 * This function will delay for some us.
 *
 * @param us the delay time of us
 */
void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}

#ifdef RT_USING_CONSOLE
void rt_hw_console_output(const char *str)
{
    while(*str)
    {
        if(*str == '\n')
          uart_putc(USART1, '\r');  
        uart_putc(USART1, *str++);
    }
}
#endif

/**
 * This function will initial STM32 board.
 */
RT_WEAK void rt_hw_board_init()
{
    std_init();

    /* System clock initialization */
    rt_system_tick_init();

    /* Heap initialization */
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif

#ifdef RT_USING_CONSOLE
    uart_init(USART1, 115200, USART_WordLength_8b, USART_StopBits_1, USART_Parity_No);
#endif
    

    /* Board underlying hardware initialization */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
}

