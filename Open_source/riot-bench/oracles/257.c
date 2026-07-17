saul_reg_t *saul_reg_find_nth(int pos)
{
    saul_reg_t *tmp = saul_reg;

    for (int i = 0; (i < pos) && tmp; i++) {
        tmp = tmp->next;
    }
    return tmp;
}
