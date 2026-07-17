int OsNotifierChainRegister(struct NotifierHead *nh,
        struct NotifierBlock *n)
{
    uintptr_t flags;
    int ret;

    flags = PRT_HwiLock();
    ret = NotifierChainRegister(&nh->head, n);
    PRT_HwiRestore(flags);
    return ret;
}
