int saul_reg_add(saul_reg_t *dev)
{
    saul_reg_t *tmp = saul_reg;

    if (dev == NULL) {
        return -ENODEV;
    }

    /* prepare new entry */
    dev->next = NULL;
    /* add to registry */
    if (saul_reg == NULL) {
        saul_reg = dev;
    }
    else {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = dev;
    }
    return 0;
}
