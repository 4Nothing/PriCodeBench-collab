int OsNotifierRawCallChain(struct NotifierHead *nh,
                   int val, void *v,
                   int nr_to_call, int *nr_calls)
{
    return NotifierCallChain(&nh->head, val, v, -1, NULL);
}
