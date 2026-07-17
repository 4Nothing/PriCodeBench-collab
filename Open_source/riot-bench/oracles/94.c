void evtimer_del(evtimer_t *evtimer, evtimer_event_t *event)
{
    unsigned state = irq_disable();

    DEBUG("evtimer_del(): removing event with offset %" PRIu32 "\n", event->offset);

    _update_head_offset(evtimer);
    _del_event_from_list(evtimer, event);
    _update_timer(evtimer);
    irq_restore(state);
}
