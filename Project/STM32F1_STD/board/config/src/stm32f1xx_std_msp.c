#include "rtthread.h"
#include "stm32f1xx_std_msp.h"

void std_init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    
#if defined(RCC_APB2ENR_IOPAEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
#endif
    
#if defined(RCC_APB2ENR_IOPBEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
#endif
    
#if defined(RCC_APB2ENR_IOPCEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
#endif
    
#if defined(RCC_APB2ENR_IOPDEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
#endif

#if defined(RCC_APB2ENR_IOPEEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
#endif

#if defined(RCC_APB2ENR_IOPFEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
#endif

#if defined(RCC_APB2ENR_IOPGEN)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG, ENABLE);
#endif
}

void uart_msp_init(USART_TypeDef *uart)
{
#ifdef BSP_USING_UART1
    if(uart == USART1)
    {
        GPIO_InitTypeDef GPIO_InitStrcture;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStrcture.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_Init(GPIOA, &GPIO_InitStrcture);

        GPIO_InitStrcture.GPIO_Pin = GPIO_Pin_10;
        GPIO_InitStrcture.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &GPIO_InitStrcture);
    }
#endif
}
