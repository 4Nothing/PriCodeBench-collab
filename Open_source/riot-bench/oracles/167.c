bool gnrc_pktbuf_is_sane(void)
{
    _unused_t *ptr = _first_unused;

    /* Invariants of this implementation:
     *  - the head of _unused_t list is _first_unused
     *  - if _unused_t list is empty the packet buffer is full and _first_unused is NULL
     *  - forall ptr_in _unused_t list: &_static_buf[0] < ptr
     *                                  && ptr < &_static_buf[CONFIG_GNRC_PKTBUF_SIZE]
     *  - forall ptr in _unused_t list: ptr->next == NULL || ptr < ptr->next
     *  - forall ptr in _unused_t list: (ptr->next != NULL && ptr->size <= (ptr->next - ptr)) ||
     *                                  (ptr->next == NULL
     *                                  && ptr->size == (CONFIG_GNRC_PKTBUF_SIZE - pos_in_buf))
     */

    while (ptr) {
        if ((&_static_buf[0] >= (uint8_t *)ptr)
            && ((uint8_t *)ptr >= &_static_buf[CONFIG_GNRC_PKTBUF_SIZE])) {
            return false;
        }
        if ((ptr->next != NULL) && (ptr >= ptr->next)) {
            return false;
        }
        size_t pos_in_buf = (uint8_t *)ptr - &_static_buf[0];
        if (((ptr->next == NULL) || (ptr->size > (size_t)((uint8_t *)(ptr->next) - (uint8_t *)ptr)))
            && ((ptr->next != NULL) || (ptr->size != CONFIG_GNRC_PKTBUF_SIZE - pos_in_buf))) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;
}