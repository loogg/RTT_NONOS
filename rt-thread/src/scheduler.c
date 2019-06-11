#include <rtthread.h>
#include <rthw.h>

#ifdef RT_USING_SMP
rt_hw_spinlock_t _rt_critical_lock;
#endif /*RT_USING_SMP*/

rt_list_t rt_thread_priority;
#ifndef RT_USING_SMP
struct rt_thread *rt_current_thread;
#endif /*RT_USING_SMP*/

rt_list_t rt_thread_defunct;

#ifdef RT_USING_HOOK
static void (*rt_scheduler_hook)(struct rt_thread *from, struct rt_thread *to);

/**
 * @addtogroup Hook
 */

/**@{*/

/**
 * This function will set a hook function, which will be invoked when thread
 * switch happens.
 *
 * @param hook the hook function
 */
void
rt_scheduler_sethook(void (*hook)(struct rt_thread *from, struct rt_thread *to))
{
    rt_scheduler_hook = hook;
}

/**@}*/
#endif

/**
 * @ingroup SystemInit
 * This function will initialize the system scheduler
 */
void rt_system_scheduler_init(void)
{
    rt_list_init(&rt_thread_priority);
    
    /* initialize thread defunct */
    rt_list_init(&rt_thread_defunct);
}

/**
 * @addtogroup Thread
 */

/**@{*/

/**
 * This function will perform one schedule. It will select one thread
 * with the highest priority level, then switch to it.
 */
void rt_schedule(void)
{
    rt_base_t level;
    rt_thread_t thread;
    rt_list_t *l;
    rt_list_for_each(l, &rt_thread_priority)
    {
        thread = rt_list_entry(l,
                               struct rt_thread,
                               tlist);
        if((thread->stat & RT_THREAD_STAT_MASK) == RT_THREAD_READY)
        {
            rt_current_thread = thread;
            thread->stat = RT_THREAD_RUNNING | (thread->stat & ~RT_THREAD_STAT_MASK);
            thread->entry(thread->parameter);
            if(thread->stat == RT_THREAD_CLOSE)
                l = rt_thread_priority.next;
            else
                thread->stat = RT_THREAD_SUSPEND | (thread->stat & ~RT_THREAD_STAT_MASK);
        }
    }
}

/**
 * This function checks if a scheduling is needed after IRQ context. If yes,
 * it will select one thread with the highest priority level, and then switch
 * to it.
 */

/*
 * This function will insert a thread to system ready queue. The state of
 * thread will be set as READY and remove from suspend queue.
 *
 * @param thread the thread to be inserted
 * @note Please do not invoke this function in user application.
 */
void rt_schedule_insert_thread(struct rt_thread *thread)
{
    register rt_base_t temp;

    RT_ASSERT(thread != RT_NULL);

    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

    /* it's current thread, it should be RUNNING thread */
    if (thread == rt_current_thread)
    {
        thread->stat = RT_THREAD_RUNNING | (thread->stat & ~RT_THREAD_STAT_MASK);
        goto __exit;
    }

    /* READY thread, insert to ready queue */
    thread->stat = RT_THREAD_READY | (thread->stat & ~RT_THREAD_STAT_MASK);
    /* insert thread to ready list */
    rt_list_insert_before(&(rt_thread_priority),
                          &(thread->tlist));

    RT_DEBUG_LOG(RT_DEBUG_SCHEDULER, ("insert thread[%.*s]\n",
                                      RT_NAME_MAX, thread->name));

__exit:
    /* enable interrupt */
    rt_hw_interrupt_enable(temp);
}

/*
 * This function will remove a thread from system ready queue.
 *
 * @param thread the thread to be removed
 *
 * @note Please do not invoke this function in user application.
 */
void rt_schedule_remove_thread(struct rt_thread *thread)
{
    register rt_base_t level;

    RT_ASSERT(thread != RT_NULL);

    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    RT_DEBUG_LOG(RT_DEBUG_SCHEDULER, ("remove thread[%.*s]\n",
                                      RT_NAME_MAX, thread->name));

    /* remove thread from ready list */
    rt_list_remove(&(thread->tlist));

    /* enable interrupt */
    rt_hw_interrupt_enable(level);
}

/**@}*/
