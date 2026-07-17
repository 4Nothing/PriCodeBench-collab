void gnrc_netreg_unregister(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
    if (_INVALID_TYPE(type)) {
        return;
    }

    _gnrc_netreg_acquire_exclusive();
    LL_DELETE(netreg[type], entry);
    /* We can release now already: No new references to this entry can be made
     * any more, and the caller is only allowed to reuse the entry and the mbox
     * target referenced by it after *this* function returned, not when the
     * lock becomes available again. */
    _gnrc_netreg_release_exclusive();

#if defined(MODULE_GNRC_NETAPI_MBOX)
    /* drain packets still in the mbox */
    if (entry->type == GNRC_NETREG_TYPE_MBOX) {
        msg_t msg;
        while (mbox_try_get(entry->target.mbox, &msg)) {
            if ((msg.type == GNRC_NETAPI_MSG_TYPE_RCV) ||
                (msg.type == GNRC_NETAPI_MSG_TYPE_SND)) {
                gnrc_pktbuf_release_error(msg.content.ptr, EBADF);
            }
        }
    }
#endif
}