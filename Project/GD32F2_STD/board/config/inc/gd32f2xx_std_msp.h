#ifndef __GD32F2X_STD_MSP_H
#define __GD32F2X_STD_MSP_H
#include "gd32f20x.h"

void std_init(void);
void uart_msp_init(uint32_t uart);
void spi_msp_init(uint32_t spi_periph);
#endif
