#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_MODULE
#include <dlmodule.h>
#endif

#if defined(RT_USING_HOOK)
#ifndef RT_USING_IDLE_HOOK
#define RT_USING_IDLE_HOOK
#endif
#endif

#ifdef RT_USING_SMP
#define _CPUS_NR RT_CPUS_NR
#else
#define _CPUS_NR 1
#endif


static struct rt_thread idle[_CPUS_NR];

#ifdef RT_USING_IDLE_HOOK
#ifndef RT_IDEL_HOOK_LIST_SIZE
#define RT_IDEL_HOOK_LIST_SIZE 4
#endif

static void (*idle_hook_list[RT_IDEL_HOOK_LIST_SIZE])();

/**
 * @ingroup Hook
 * This function sets a hook function to idle thread loop. When the system performs
 * idle loop, this hook function should be invoked.
 *
 * @param hook the specified hook function
 *
 * @return RT_EOK: set OK
 *         -RT_EFULL: hook list is full
 *
 * @note the hook function must be simple and never be blocked or suspend.
 */
rt_err_t rt_thread_idle_sethook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;
    rt_err_t ret = -RT_EFULL;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDEL_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == RT_NULL)
        {
            idle_hook_list[i] = hook;
            ret = RT_EOK;
            break;
        }
    }
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return ret;
}

/**
 * delete the idle hook on hook list
 *
 * @param hook the specified hook function
 *
 * @return RT_EOK: delete OK
 *         -RT_ENOSYS: hook was not found
 */
rt_err_t rt_thread_idle_delhook(void (*hook)(void))
{
    rt_size_t i;
    rt_base_t level;
    rt_err_t ret = -RT_ENOSYS;

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    for (i = 0; i < RT_IDEL_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] == hook)
        {
            idle_hook_list[i] = RT_NULL;
            ret = RT_EOK;
            break;
        }
    }
    /* enable interrupt */
    rt_hw_interrupt_enable(level);

    return ret;
}

#endif



static void rt_thread_idle_entry(void *parameter)
{

#ifdef RT_USING_IDLE_HOOK
    rt_size_t i;

    for (i = 0; i < RT_IDEL_HOOK_LIST_SIZE; i++)
    {
        if (idle_hook_list[i] != RT_NULL)
        {
            idle_hook_list[i]();
        }
    }
#endif
}

/**
 * @ingroup SystemInit
 *
 * This function will initialize idle thread, then start it.
 *
 * @note this function must be invoked when system init.
 */
void rt_thread_idle_init(void)
{
    rt_ubase_t i;

    for (i = 0; i < _CPUS_NR; i++)
    {
        rt_thread_init(&idle[i],
                       rt_thread_idle_entry,
                       RT_NULL,
                       0);
        /* startup */
        rt_thread_startup(&idle[i]);
    }
}

/**
 * @ingroup Thread
 *
 * This function will get the handler of the idle thread.
 *
 */
rt_thread_t rt_thread_idle_gethandler(void)
{
#ifdef RT_USING_SMP
    register int id = rt_hw_cpu_id();
#else
    register int id = 0;
#endif

    return (rt_thread_t)(&idle[id]);
}
