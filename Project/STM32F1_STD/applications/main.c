#include <rtthread.h>
#include <board.h>
#include "msh.h"
#include "mem.h"

static void main_loop(void)
{
    rt_bool_t abort = RT_FALSE;
    int bootdelay = g_param.bootdelay;
    char ch = 0;
    char *bootcmd = g_param.bootcmd;
    if(bootdelay < 0)
    {
        goto _exit;
    }
    rt_kprintf("Hit CTRL+C to stop autoboot: %2d ", bootdelay);

    if (bootdelay == 0)
    {
        if (uart_getc(rt_console_get_uart(), &ch) && (ch == 3))
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
            if (uart_getc(rt_console_get_uart(), &ch) && (ch == 3))
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
        if(msh_exec(bootcmd, rt_strlen(bootcmd)) < 0)
			return;
        while (1)
            ;
    }
}

int main(void)
{
    main_loop();
    while(1)
    {
        rt_schedule();
    }
}
