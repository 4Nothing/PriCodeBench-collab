static inline thread_t *thread_get_active(void)
{
    extern volatile thread_t *sched_active_thread;

    return (thread_t *)sched_active_thread;
}
