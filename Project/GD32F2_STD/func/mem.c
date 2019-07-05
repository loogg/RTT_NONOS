#include "mem.h"
#include "stdlib.h"
#include "string.h"

#define DBG_ENABLE
#define DBG_LVL    DBG_LOG
#define DBG_TAG     "mem"
#include "rtdbg.h"

g_param_t g_param;

static void mem_param_default_init(void)
{
    g_param.bootdelay = 5;
    rt_memset(g_param.bootcmd, 0, sizeof(g_param.bootcmd));
    rt_memcpy(g_param.bootcmd, "boot -p 8005000", rt_strlen("boot -p 8005000"));
    g_param.magic = SAVA_MAGIC;
}

static void mem_param_update(void)
{
    LOG_D("Flash is erasing...");
    if(gd32_flash_erase(FLASH_SAVE_ADDR, MAX_SAVE_LEN) < 0)
    {
        LOG_E("Flash erase error!");
        return;
    }
    LOG_D("Flash erase success");
    LOG_D("Flash is writing...");
    if(gd32_flash_write(FLASH_SAVE_ADDR, (const rt_uint8_t *)g_param.data, MAX_SAVE_LEN) < 0)
    {
        LOG_E("Flash write error!");
    }
    LOG_D("Flash write success");
}
MSH_CMD_EXPORT_ALIAS(mem_param_update, saveenv, save param to flash);

static void mem_print(void)
{
    rt_kprintf("\r\n");
    rt_kprintf("bootdelay = %d\r\n", g_param.bootdelay);
    rt_kprintf("bootcmd = %s\r\n", g_param.bootcmd);
    rt_kprintf("\r\n");
}
MSH_CMD_EXPORT_ALIAS(mem_print, printenv, print param to console);

static void mem_set(int argc, char**argv)
{
    if(argc < 3)
    {
        rt_kprintf("Usage: setenv <param> <value>.\n");
    }
    else
    {
        char *param = argv[1];
        if(rt_strcmp(param, "bootdelay") && rt_strcmp(param, "bootcmd"))
        {
            rt_kprintf("param: not support.\n");
        }
        else
        {
            if(!rt_strcmp(param, "bootdelay"))
            {
                g_param.bootdelay = atoi(argv[2]);
            }
            else if(!rt_strcmp(param, "bootcmd"))
            {
                rt_memset(g_param.bootcmd, 0, sizeof(g_param.bootcmd));
                rt_memcpy(g_param.bootcmd, argv[2], rt_strlen(argv[2]));
                for(uint8_t i = 3; i < argc; i++)
                {
                    strcat(g_param.bootcmd, " ");
                    strcat(g_param.bootcmd, argv[i]);
                }
                
            }
        }
        
    }
    
}
MSH_CMD_EXPORT_ALIAS(mem_set, setenv, set param value);

int mem_param_init(void)
{
    rt_memset(&g_param, 0, sizeof(g_param));
    gd32_flash_read(FLASH_SAVE_ADDR, (rt_uint8_t *)g_param.data, MAX_SAVE_LEN);
    if(g_param.magic != SAVA_MAGIC)
    {
        LOG_W("Param check error, now set to default");
        mem_param_default_init();
        mem_param_update();
    }
	return 0;
}

INIT_ENV_EXPORT(mem_param_init); 

