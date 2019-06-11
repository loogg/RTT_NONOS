#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdlib.h>
#include "easyflash.h"
#include "fal.h"
#include "msh.h"

static void main_loop(void)
{
    rt_bool_t abort = RT_FALSE;
    rt_device_t dev = rt_console_get_device();
    int bootdelay = atoi(ef_get_env("bootdelay"));
    char ch = 0;
    if(bootdelay < 0)
    {
        goto _exit;
    }
    rt_kprintf("Hit any key to stop autoboot: %2d ", bootdelay);

    if (bootdelay == 0)
    {
        if (rt_device_read(dev, 0, &ch, 1))
        {              
            rt_kprintf("\b\b\b 0");
            abort = RT_TRUE; /* don't auto boot	*/
        }
    }

    while ((bootdelay > 0) && (!abort))
    {
        int i;
        --bootdelay;
        for (i = 0; !abort && i < 100; ++i)
        {
            if (rt_device_read(dev, 0, &ch, 1))
            {                  
                abort = RT_TRUE;     /* don't auto boot	*/
                bootdelay = 0; /* no more delay	*/
                break;
            }
            rt_tick_delay(10);
        }

        rt_kprintf("\b\b\b%2d ", bootdelay);
    }
    rt_kprintf("\n");

_exit:
    if (!abort)
    {
        char *bootcmd = ef_get_env("bootcmd");
        msh_exec(bootcmd, rt_strlen(bootcmd));
        while (1)
            ;
    }
}

int main(void)
{
    fal_init();
    easyflash_init();
    main_loop();
    while(1)
    {
        rt_schedule();
    }
}
