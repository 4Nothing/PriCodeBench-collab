int gnrc_ipv6_nib_nc_set(const ipv6_addr_t *ipv6, unsigned iface,
                         const uint8_t *l2addr, size_t l2addr_len)
{
    _nib_onl_entry_t *node;

    assert(ipv6 != NULL);
    assert(l2addr_len <= CONFIG_GNRC_IPV6_NIB_L2ADDR_MAX_LEN);
    assert((iface > KERNEL_PID_UNDEF) && (iface <= KERNEL_PID_LAST));
    _nib_acquire();
    node = _nib_nc_add(ipv6, iface, GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    if (node == NULL) {
        _nib_release();
        return -ENOMEM;
    }
#if IS_ACTIVE(CONFIG_GNRC_IPV6_NIB_ARSM)
    if ((l2addr != NULL) && (l2addr_len > 0)) {
        memcpy(node->l2addr, l2addr, l2addr_len);
    }
    node->l2addr_len = l2addr_len;
#else
    (void)l2addr;
    (void)l2addr_len;
    if (!ipv6_addr_is_link_local(ipv6)) {
        return -EINVAL;
    }
#endif
    node->info &= ~(GNRC_IPV6_NIB_NC_INFO_AR_STATE_MASK |
                    GNRC_IPV6_NIB_NC_INFO_NUD_STATE_MASK);
    node->info |= (GNRC_IPV6_NIB_NC_INFO_AR_STATE_MANUAL |
                   GNRC_IPV6_NIB_NC_INFO_NUD_STATE_UNMANAGED);
    _nib_release();
    return 0;
}
