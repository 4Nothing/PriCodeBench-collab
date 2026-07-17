gnrc_netreg_entry_t *gnrc_netreg_getnext(gnrc_netreg_entry_t *entry)
{
    return (entry ? _netreg_lookup(entry, 0, entry->demux_ctx) : NULL);
}