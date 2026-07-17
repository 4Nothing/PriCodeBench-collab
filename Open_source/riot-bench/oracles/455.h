static inline clist_node_t *clist_rpop(clist_node_t *list)
{
    if (list->next) {
        list_node_t *last = list->next;
        while (list->next->next != last) {
            clist_lpoprpush(list);
        }
        return clist_lpop(list);
    }
    else {
        return NULL;
    }
}
