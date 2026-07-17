static inline unsigned gnrc_ipv6_nib_nc_get_iface(const gnrc_ipv6_nib_nc_t *entry)
{
    return (entry->info & GNRC_IPV6_NIB_NC_INFO_IFACE_MASK) >>
           GNRC_IPV6_NIB_NC_INFO_IFACE_POS;
}
