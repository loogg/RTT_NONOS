#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "fal.h"
#include "easyflash.h"
#include "at.h"

static rt_thread_t test_tid =RT_NULL;
at_response_t resp = RT_NULL;
static void test(void *param)
{
    if(at_exec_cmd(resp, "AT+CWLAP") == RT_EOK)
    {
        for (uint8_t i = 0; i < (int)resp->line_counts; i++)
        {
            rt_kprintf("%s\r\n", at_resp_get_line(resp, i + 1));
        }
    }
}
    
int main(void)
{
    fal_init();
    easyflash_init();
    at_client_init("uart2", 512);
    resp = at_create_resp(1024, 0, 5000);
    test_tid = rt_thread_create("test", test, RT_NULL, 1000);
    rt_thread_startup(test_tid);
    while(1)
    {
        rt_schedule();
    }
}
