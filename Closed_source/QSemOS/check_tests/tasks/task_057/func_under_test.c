static int NotifierChainRegister(struct NotifierBlock **nl,
        struct NotifierBlock *n)
{
    while ((*nl) != NULL) {
        if (*nl == n) {
            // double register
            return -1;
        }
        if (n->priority > (*nl)->priority) {
            break;
        }
        nl = &((*nl)->next);
    }
    n->next = *nl;
    *nl = n;
    return 0;
}
