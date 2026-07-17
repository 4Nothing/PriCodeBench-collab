int gnrc_netreg_register(gnrc_nettype_t type, gnrc_netreg_entry_t *entry)
{
#if DEVELHELP
# if defined(MODULE_GNRC_NETAPI_MBOX) || defined(MODULE_GNRC_NETAPI_CALLBACKS)
    bool has_msg_q = (entry->type != GNRC_NETREG_TYPE_DEFAULT) ||
                     thread_has_msg_queue(thread_get(entry->target.pid));
# else
    bool has_msg_q = thread_has_msg_queue(thread_get(entry->target.pid));
# endif

    /* only threads with a message queue are allowed to register at gnrc */
    if (!has_msg_q) {
        LOG_ERROR("\n!!!! gnrc_netreg: initialize message queue of thread %u "
                  "using msg_init_queue() !!!!\n\n", entry->target.pid);
    }
    assert(has_msg_q);
#endif /* DEVELHELP */

    if (_INVALID_TYPE(type)) {
        return -EINVAL;
    }

    _gnrc_netreg_acquire_exclusive();

    /* don't add the same entry twice */
    gnrc_netreg_entry_t *e;
    LL_FOREACH(netreg[type], e) {
        assert(entry != e);
    }

    LL_PREPEND(netreg[type], entry);
    _gnrc_netreg_release_exclusive();

    return 0;
}