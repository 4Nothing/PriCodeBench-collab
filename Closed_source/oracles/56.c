static int NotifierCallChain(struct NotifierBlock **nl,
                   int val, void *v,
                   int nr_to_call, int *nr_calls)
{
    int ret = NOTIFY_DONE;
    struct NotifierBlock *nb, *next_nb;

    nb = *nl;

    while (nb && nr_to_call) {
        next_nb = nb->next;

        ret = nb->call(nb, val, v);

        if (nr_calls) {
            (*nr_calls)++;
        }

        if (ret & NOTIFY_STOP_MASK) {
            break;
        }
        nb = next_nb;
        nr_to_call--;
    }
    return ret;
}
