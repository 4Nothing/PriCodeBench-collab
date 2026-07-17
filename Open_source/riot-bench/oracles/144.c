void gnrc_ipv6_nib_init(void)
{
    evtimer_event_t *tmp;

    _nib_acquire();
    for (evtimer_event_t *ptr = _nib_evtimer.events;
         (ptr != NULL) && (tmp = (ptr->next), 1);
         ptr = tmp) {
        evtimer_del((evtimer_t *)(&_nib_evtimer), ptr);
    }
    _nib_init();
    _nib_release();
}
