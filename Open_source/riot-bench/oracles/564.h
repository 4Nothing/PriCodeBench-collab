static inline int mbox_try_put(mbox_t *mbox, msg_t *msg)
{
    return _mbox_put(mbox, msg, NON_BLOCKING);
}