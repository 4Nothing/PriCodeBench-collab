saul_reg_t *saul_reg_find_type_and_name(uint8_t type, const char *name)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (tmp->driver->type == type && strcmp(tmp->name, name) == 0) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}
