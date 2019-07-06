#ifndef __MEM_H
#define __MEM_H
#include "board.h"

#define FLASH_SAVE_ADDR 0x08004800
#define MAX_SAVE_LEN    0x800
#define SAVA_MAGIC      0xa5a5a5a5

#pragma anon_unions
typedef union 
{
    uint32_t data[MAX_SAVE_LEN/4];
    struct
    {
        int bootdelay;
        char bootcmd[100];
        uint32_t magic;
    };
}g_param_t;

extern g_param_t g_param;
#endif
