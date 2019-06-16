/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date         Author    Notes
 * 2009-10-11   Bernard   First version
 * 2010-12-29   onelife   Modify for EFM32
 * 2011-06-17   onelife   Merge all of the assembly source code into context_gcc.S
 * 2011-07-12   onelife   Add interrupt context check function
 * 2013-06-18   aozima    add restore MSP feature.
 * 2013-07-09   aozima    enhancement hard fault exception handler.
 */
 
    .cpu    cortex-m3
    .fpu    softvfp
    .syntax unified
    .thumb
    .text

    .equ    SCB_VTOR, 0xE000ED08            /* Vector Table Offset Register */
    .equ    ICSR, 0xE000ED04                /* interrupt control state register */
    .equ    PENDSVSET_BIT, 0x10000000       /* value to trigger PendSV exception */
    
    .equ    SHPR3, 0xE000ED20               /* system priority register (3) */
    .equ    PENDSV_PRI_LOWEST, 0x00FF0000   /* PendSV priority value (lowest) */

/*
 * rt_base_t rt_hw_interrupt_disable();
 */
    .global rt_hw_interrupt_disable
    .type rt_hw_interrupt_disable, %function
rt_hw_interrupt_disable:
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR

/*
 * void rt_hw_interrupt_enable(rt_base_t level);
 */
    .global rt_hw_interrupt_enable
    .type rt_hw_interrupt_enable, %function
rt_hw_interrupt_enable:
    MSR     PRIMASK, R0
    BX      LR

