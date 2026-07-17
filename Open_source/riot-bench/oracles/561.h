static inline size_t mbox_avail(mbox_t *mbox)
{
    return cib_avail(&mbox->cib);
}