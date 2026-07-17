static int NotifierChainUnregister(struct NotifierBlock **nl,
        struct NotifierBlock *n)
{
    while (*nl != NULL) {
        if (*nl == n) {
            *nl = n->next;
            return 0;
        }
        nl = &((*nl)->next);
    }
    return -1;
}
