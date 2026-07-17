void gnrc_sixlowpan_frag_vrb_gc(void)
{
    uint32_t now_usec = xtimer_now_usec();

    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        if (!gnrc_sixlowpan_frag_vrb_entry_empty(&_vrb[i]) &&
            (now_usec - _vrb[i].super.arrival) > CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_TIMEOUT_US) {
            DEBUG("6lo vrb: entry (%s, ",
                  gnrc_netif_addr_to_str(_vrb[i].super.src,
                                         _vrb[i].super.src_len,
                                         addr_str));
            DEBUG("%s, %u, %u) timed out\n",
                  gnrc_netif_addr_to_str(_vrb[i].super.dst,
                                         _vrb[i].super.dst_len,
                                         addr_str),
                  (unsigned)_vrb[i].super.datagram_size, _vrb[i].super.tag);
            gnrc_sixlowpan_frag_vrb_rm(&_vrb[i]);
        }
    }
}
