#include <rthw.h>
#include <rtthread.h>

#ifdef RT_USING_SMP
#define rt_tick rt_cpu_index(0)->tick
#else
static volatile rt_tick_t rt_tick = 0;
#endif

/**
 * This function will init system tick and set it to zero.
 * @ingroup SystemInit
 *
 * @deprecated since 1.1.0, this function does not need to be invoked
 * in the system initialization.
 */
RT_WEAK void rt_system_tick_init(void)
{
}

/**
 * @addtogroup Clock
 */

/**@{*/

/**
 * This function will return current tick from operating system startup
 *
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}
RTM_EXPORT(rt_tick_get);

/**
 * This function will set current tick
 */
void rt_tick_set(rt_tick_t tick)
{
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_tick = tick;
    rt_hw_interrupt_enable(level);
}

/**
 * This function will notify kernel there is one tick passed. Normally,
 * this function is invoked by clock ISR.
 */
void rt_tick_increase(void)
{
    struct rt_thread *thread;

    /* increase the global tick */
#ifdef RT_USING_SMP
    rt_cpu_self()->tick ++;
#else
    ++ rt_tick;
#endif

    extern rt_list_t rt_thread_priority;
    rt_list_t *l;
    rt_list_for_each(l, &rt_thread_priority)
    {
        thread = rt_list_entry(l,
                               struct rt_thread,
                               tlist);
        if(((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_SUSPEND) && (thread->remaining_tick >= 0))
        {  
            if(thread->remaining_tick == 0)
            {
                thread->remaining_tick = thread->init_tick;
                rt_thread_timeout(thread);
                continue;
            }
            thread->remaining_tick --;
        }
    }
}

/**
 * This function will calculate the tick from millisecond.
 *
 * @param ms the specified millisecond
 *           - Negative Number wait forever
 *           - Zero not wait
 *           - Max 0x7fffffff
 *
 * @return the calculated tick
 */
rt_tick_t rt_tick_from_millisecond(rt_int32_t ms)
{
    rt_tick_t tick;

    if (ms < 0)
    {
        tick = (rt_tick_t)RT_WAITING_FOREVER;
    }
    else
    {
        tick = RT_TICK_PER_SECOND * (ms / 1000);
        tick += (RT_TICK_PER_SECOND * (ms % 1000) + 999) / 1000;
    }
    
    /* return the calculated tick */
    return tick;
}
RTM_EXPORT(rt_tick_from_millisecond);

/**
 * This function will blocking delay thread.
 */
void rt_tick_delay(rt_tick_t tick)
{
    rt_tick_t timeout_tick = rt_tick_get() + tick;
    while((rt_tick_get() - timeout_tick) >= RT_TICK_MAX / 2);
}

/**@}*/

