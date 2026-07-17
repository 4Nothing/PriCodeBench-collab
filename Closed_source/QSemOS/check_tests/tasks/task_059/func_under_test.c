int OsNotifierCallChain(struct NotifierHead *nh,
                   int val, void *v,
                   int nr_to_call, int *nr_calls)
{
    uintptr_t flags;
    int ret;

    flags = PRT_HwiLock();
    ret = NotifierCallChain(&nh->head, val, v, -1, NULL);
    PRT_HwiRestore(flags);
    return ret;
}
