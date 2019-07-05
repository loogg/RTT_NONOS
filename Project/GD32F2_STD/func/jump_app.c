#include "jump_app.h"
#include "util.h"

#define DBG_ENABLE
#define DBG_LVL    DBG_LOG
#define DBG_TAG     "jump_app"
#include "rtdbg.h"

static void jump_to_app(uint32_t app_addr)
{
    uint32_t sp_init_val;
    uint32_t jump_addr;

    void (*jump_fun)(void);
    SysTick->CTRL&=~(SysTick_CTRL_ENABLE_Msk|SysTick_CTRL_TICKINT_Msk);
    __disable_irq();

    sp_init_val = *(uint32_t *)app_addr;
    jump_addr = *(uint32_t *)(app_addr + 4);

    __set_MSP(sp_init_val);
    jump_fun = (void (*)(void))jump_addr;
    (*jump_fun)();

}

static rt_err_t app_firm_jump(uint32_t app_addr)
{
    if(((*(uint32_t *)app_addr) & 0x2FFE0000) != 0x20000000)
    {
        LOG_E("Find user firmware at 0x%08x failed.", app_addr);
        return -RT_ERROR;
    }
    if(((*(uint32_t *)(app_addr+4)) & 0xFF000000) != 0x08000000)
    {
        LOG_E("Find user firmware at 0x%08x failed.", app_addr);
        return -RT_ERROR;
    }

    LOG_I("Find user firmware at 0x%08x successfully.", app_addr);
    LOG_I("Bootloader jumps tp user firmware now.");
    jump_to_app(app_addr);
	return RT_EOK;
}

static rt_err_t boot(int argc, char**argv)
{
    if(argc < 3)
    {
        rt_kprintf("Usage: boot -p <address>.\n");
        return -RT_ERROR;
    }
    else
    {
        const char *operator = argv[1];
        if (!strcmp(operator, "-p"))
        {

            /* change default partition to save firmware */
            char *recv_addr = argv[2];
            uint32_t addr = htoi(recv_addr);
            return app_firm_jump(addr);
        }
        else
        {
            rt_kprintf("Usage: ymodem_ota -p <address>.\n");
            return -RT_ERROR;
        }
    }
    
}
MSH_CMD_EXPORT(boot, Use boot to jump the firmware);


