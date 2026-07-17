static inline int mbox_try_get(mbox_t *mbox, msg_t *msg)
{
    return _mbox_get(mbox, msg, NON_BLOCKING);
}