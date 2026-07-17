static inline unsigned gnrc_ipv6_nib_nc_get_nud_state(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
}
