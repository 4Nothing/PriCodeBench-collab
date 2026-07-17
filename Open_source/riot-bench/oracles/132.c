int gcoap_get_resource_list(void *buf, size_t maxlen, uint8_t cf,
                               gcoap_socket_type_t tl_type)
{
    assert(cf == COAP_FORMAT_LINK);

    gcoap_listener_t *listener = _coap_state.listeners;

    char *out = (char *)buf;
    size_t pos = 0;

    coap_link_encoder_ctx_t ctx;
    ctx.content_format = cf;
    /* indicate initial link for the list */
    ctx.flags = COAP_LINK_FLAG_INIT_RESLIST;

    /* write payload */
    for (; listener != NULL; listener = listener->next) {
        if (!listener->link_encoder) {
            continue;
        }
        /* only makes sense to check if non-UDP transports are supported,
         * so check if module is used first. */
        if (IS_USED(MODULE_GCOAP_DTLS) &&
            (tl_type != GCOAP_SOCKET_TYPE_UNDEF) &&
            (listener->tl_type != GCOAP_SOCKET_TYPE_UNDEF) &&
            ((listener->tl_type & GCOAP_SOCKET_TYPE_UDP) != (tl_type & GCOAP_SOCKET_TYPE_UDP)) &&
            ((listener->tl_type & GCOAP_SOCKET_TYPE_DTLS) != (tl_type & GCOAP_SOCKET_TYPE_DTLS))) {
            continue;
        }
        ctx.link_pos = 0;

        for (; ctx.link_pos < listener->resources_len; ctx.link_pos++) {
            ssize_t res;
            if (out) {
                res = listener->link_encoder(&listener->resources[ctx.link_pos],
                                             &out[pos], maxlen - pos, &ctx);
            }
            else {
                res = listener->link_encoder(&listener->resources[ctx.link_pos],
                                             NULL, 0, &ctx);
            }

            if (res > 0) {
                pos += res;
                ctx.flags &= ~COAP_LINK_FLAG_INIT_RESLIST;
            }
            else {
                break;
            }
        }
    }

    return (int)pos;
}
