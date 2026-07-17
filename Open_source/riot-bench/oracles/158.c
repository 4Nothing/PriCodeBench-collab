gnrc_netreg_entry_t *gnrc_netreg_lookup(gnrc_nettype_t type, uint32_t demux_ctx)
{
    return _netreg_lookup(NULL, type, demux_ctx);
}