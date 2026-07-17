saul_reg_t *saul_reg_find_name(const char *name)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (strcmp(tmp->name, name) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}
