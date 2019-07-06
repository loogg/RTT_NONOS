/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-30     armink       the first version
 * 2018-08-27     Murphy       update log
 */

#include <rtthread.h>
#include "board.h"
#include <finsh.h>
#include <ymodem.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME               "ymodem"
#ifdef OTA_DOWNLOADER_DEBUG
#define DBG_LEVEL                      DBG_LOG
#else
#define DBG_LEVEL                      DBG_INFO
#endif
#define DBG_COLOR
#include <rtdbg.h>


static size_t update_file_total_size, update_file_cur_size;
static uint32_t addr;
static char *recv_addr = RT_NULL;

struct rym_ctx rctx = {0};

static int htoi(char s[])  
{  
    int i;  
    int n = 0;  
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))  
    {  
        i = 2;  
    }  
    else  
    {  
        i = 0;  
    }  
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z');++i)  
    {  
        if (tolower(s[i]) > '9')  
        {  
            n = 16 * n + (10 + tolower(s[i]) - 'a');  
        }  
        else  
        {  
            n = 16 * n + (tolower(s[i]) - '0');  
        }  
    }  
    return n;  
}  

static enum rym_code ymodem_on_begin(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    char *file_name, *file_size;

    /* calculate and store file size */
    file_name = (char *)&buf[0];
    file_size = (char *)&buf[rt_strlen(file_name) + 1];
    update_file_total_size = atol(file_size);
    rt_kprintf("Ymodem file_size:%d\n", update_file_total_size);

    update_file_cur_size = 0;

    
    if (update_file_total_size > STM32_FLASH_SIZE)
    {
        LOG_E("Firmware is too large! File size (%d)", update_file_total_size);
        return RYM_CODE_CAN;
    }

    LOG_I("Start erase. Size (%d)", update_file_total_size);

    /* erase DL section */
    if (stm32_flash_erase(addr,update_file_total_size) < 0)
    {
        LOG_E("Firmware download failed! erase error!");
        return RYM_CODE_CAN;
    }

    return RYM_CODE_ACK;
}

static enum rym_code ymodem_on_data(struct rym_ctx *ctx, rt_uint8_t *buf, rt_size_t len)
{
    /* write data of application to DL partition  */
    if (stm32_flash_write(addr + update_file_cur_size, buf, len) < 0)
    {
        LOG_E("Firmware download failed! write data error!");
        return RYM_CODE_CAN;
    }

    update_file_cur_size += len;

    return RYM_CODE_ACK;
}

int ymodem_ota(uint8_t argc, char **argv)
{
    if (argc < 2)
    {
        rt_kprintf("Default save firmware on download partition.\n");
    }
    else
    {
        const char *operator = argv[1];
        if (!strcmp(operator, "-p")) {
            if (argc < 3) {
                rt_kprintf("Usage: ymodem_ota -p <address>.\n");
                return -RT_ERROR;
            } else {
                /* change default partition to save firmware */
                recv_addr = argv[2];  
                addr = htoi(recv_addr);
                if(addr < (STM32_FLASH_START_ADRESS + 20*1024))
                {
                    LOG_E("Address is too small! Please input more than (%x)", STM32_FLASH_START_ADRESS + 20*1024);
                    return -RT_ERROR;
                }
            }
        }else{
            rt_kprintf("Usage: ymodem_ota -p <address>.\n");
            return -RT_ERROR;
        }

        rt_kprintf("Warning: Ymodem has started! This operator will not recovery.\n");
        rt_kprintf("Please select the ota firmware file and use Ymodem to send.\n");

        if (!rym_recv_on_device(rt_console_get_uart(), &rctx, 
                                ymodem_on_begin, ymodem_on_data, NULL, RT_TICK_PER_SECOND))
        {
            rt_tick_delay(5);
            rt_kprintf("Download firmware to flash success.\n");     
        }
        else
        {
            /* wait some time for terminal response finish */
            rt_tick_delay(RT_TICK_PER_SECOND);
            rt_kprintf("Update firmware fail.\n");
            return -RT_ERROR;
        }
    }

    return RT_EOK;
}
/**
 * msh />ymodem_ota
*/
MSH_CMD_EXPORT(ymodem_ota, Use Y-MODEM to download the firmware);

