saul_reg_t *saul_reg_find_type(uint8_t type)
{
    saul_reg_t *tmp = saul_reg;

    while (tmp) {
        if (tmp->driver->type == type) {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}
