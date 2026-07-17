void bhp_event_init(bhp_event_t *bhp_ev, event_queue_t *evq, bhp_cb_t cb, void *ctx)
{
    bhp_set_cb(&bhp_ev->bhp, cb, ctx);
    bhp_ev->evq = evq;
    bhp_ev->ev.handler = _event_handler;
}
