#include <rtthread.h>

rt_list_t rt_thread_priority;

struct rt_thread *rt_current_thread;

/**
 * @ingroup SystemInit
 * This function will initialize the system scheduler
 */
void rt_system_scheduler_init(void)
{
    rt_list_init(&rt_thread_priority);
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
    rt_thread_t thread;
    rt_list_t *l;
    rt_list_for_each(l, &rt_thread_priority)
    {
        thread = rt_list_entry(l,
                               struct rt_thread,
                               tlist);
        if(thread->stat == RT_THREAD_READY)
        {
            rt_current_thread = thread;
            thread->stat = RT_THREAD_RUNNING ;
            thread->entry(thread->parameter);
            if(thread->stat == RT_THREAD_CLOSE)
                l = rt_thread_priority.next;
            else
                thread->stat = RT_THREAD_SUSPEND;
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
    RT_ASSERT(thread != RT_NULL);
    /* it's current thread, it should be RUNNING thread */
    if (thread == rt_current_thread)
    {
        thread->stat = RT_THREAD_RUNNING;
    }

    /* READY thread, insert to ready queue */
    thread->stat = RT_THREAD_READY;
    /* insert thread to ready list */
    rt_list_insert_before(&(rt_thread_priority),
                          &(thread->tlist));

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
    RT_ASSERT(thread != RT_NULL);
    /* remove thread from ready list */
    rt_list_remove(&(thread->tlist));

}

/**@}*/
