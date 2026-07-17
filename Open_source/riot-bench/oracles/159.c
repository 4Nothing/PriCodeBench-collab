int gnrc_netreg_num(gnrc_nettype_t type, uint32_t demux_ctx)
{
    int num = 0;
    gnrc_netreg_entry_t *entry = NULL;

    gnrc_netreg_acquire_shared();

    while((entry = _netreg_lookup(entry, type, demux_ctx)) != NULL) {
        num++;
    }

    gnrc_netreg_release_shared();

    return num;
}