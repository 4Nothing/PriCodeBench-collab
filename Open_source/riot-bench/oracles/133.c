void gcoap_register_listener(gcoap_listener_t *listener)
{
    /* That item will be overridden, ensure that the user expecting different
     * behavior will notice this. */
    assert(listener->next == NULL);

    listener->next = _coap_state.listeners;
    _coap_state.listeners = listener;

    if (!listener->link_encoder) {
        listener->link_encoder = gcoap_encode_link;
    }

    if (!listener->request_matcher) {
        listener->request_matcher = _request_matcher_default;
    }
}
