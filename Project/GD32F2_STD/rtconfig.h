#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Memory Management */

#define RT_USING_NOHEAP

/* Kernel Device Object */

#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_VER_NUM 0x40001
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M3

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_USING_MSH_ONLY
#define FINSH_ARG_MAX 10

/* Device Drivers */


/* Using WiFi */


/* Using USB */


/* Network */

/* Socket abstraction layer */


/* Network interface device */


/* light weight TCP/IP stack */


/* Modbus master and slave stack */


/* AT commands */


/* Utilities */

#define RT_USING_RYM

/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */


/* peripheral libraries and drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */

#define SOC_FAMILY_GD32
#define SOC_SERIES_GD32F2

/* Hardware Drivers Config */

#define SOC_GD32F2X

/* Onboard Peripheral Drivers */


/* On-chip Peripheral Drivers */

#define BSP_USING_UART_485
#define BSP_USING_UART
#define BSP_USING_UART2
#define BSP_UART2_485
#define BSP_USING_ON_CHIP_FLASH
#define ON_CHIP_FLASH_MAX_ERASE_SIZE 2048

/* Board extended module Drivers */


#endif
