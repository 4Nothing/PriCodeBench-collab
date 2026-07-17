void thread_zombify(void)
{
    if (irq_is_in()) {
        return;
    }

    irq_disable();
    sched_set_status(thread_get_active(), STATUS_ZOMBIE);
    irq_enable();
    thread_yield_higher();

    /* this line should never be reached */
    UNREACHABLE();
}
