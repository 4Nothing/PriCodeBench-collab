void gnrc_netreg_init(void)
{
    /* set all pointers in registry to NULL */
    memset(netreg, 0, GNRC_NETTYPE_NUMOF * sizeof(gnrc_netreg_entry_t *));
}