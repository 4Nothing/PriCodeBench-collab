void auto_init_event_thread(void)
{
    if (IS_USED(MODULE_EVENT_THREAD_HIGHEST)) {
        /* In order to allow highest priority events to preempt all others,
         * high priority events must be run in their own thread. This thread
         * can preempt than preempt the other event thread(s). */
        event_thread_init(EVENT_PRIO_HIGHEST,
                          _evq_highest_stack, sizeof(_evq_highest_stack),
                          EVENT_THREAD_HIGHEST_PRIO);
    }
    if (IS_USED(MODULE_EVENT_THREAD_MEDIUM)) {
        /* In order to allow medium priority events to preempt low priority
         * events, we need to move the low priority events into their own
         * thread. The always existing medium priority event thread can then
         * preempt the lowest priority event thread. */
        event_thread_init(EVENT_PRIO_LOWEST,
                          _evq_lowest_stack, sizeof(_evq_lowest_stack),
                          EVENT_THREAD_LOWEST_PRIO);
    }

    event_queue_t *qs = EVENT_PRIO_MEDIUM;
    size_t qs_numof = 1;
    if (!IS_USED(MODULE_EVENT_THREAD_HIGHEST)) {
        qs = EVENT_PRIO_HIGHEST;
        qs_numof = 2;
    }
    if (!IS_USED(MODULE_EVENT_THREAD_MEDIUM)) {
        qs_numof++;
    }
    event_thread_init_multi(qs, qs_numof,
                            _evq_medium_stack, sizeof(_evq_medium_stack),
                            EVENT_THREAD_MEDIUM_PRIO);
}
