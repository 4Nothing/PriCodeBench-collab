static inline void clist_lpoprpush(clist_node_t *list)
{
    if (list->next) {
        list->next = list->next->next;
    }
}
