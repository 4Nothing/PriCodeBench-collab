gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_add(
        const gnrc_sixlowpan_frag_rb_base_t *base,
        gnrc_netif_t *out_netif, const uint8_t *out_dst, size_t out_dst_len)
{
    gnrc_sixlowpan_frag_vrb_t *vrbe = NULL;

    assert(base != NULL);
    assert(base->src_len != 0);
    assert(out_netif != NULL);
    assert(out_dst != NULL);
    assert(out_dst_len > 0);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        gnrc_sixlowpan_frag_vrb_t *ptr = &_vrb[i];

        if (gnrc_sixlowpan_frag_vrb_entry_empty(ptr) ||
            _equal_index(ptr, base->src, base->src_len, base->tag)) {
            vrbe = ptr;
            if (gnrc_sixlowpan_frag_vrb_entry_empty(vrbe)) {
                vrbe->super = *base;
                vrbe->out_netif = out_netif;
                memcpy(vrbe->super.dst, out_dst, out_dst_len);
                vrbe->out_tag = gnrc_sixlowpan_frag_fb_next_tag();
                vrbe->super.dst_len = out_dst_len;
                DEBUG("6lo vrb: creating entry (%s, ",
                      gnrc_netif_addr_to_str(vrbe->super.src,
                                             vrbe->super.src_len,
                                             addr_str));
                DEBUG("%s, %u, %u) => ",
                      gnrc_netif_addr_to_str(vrbe->super.dst,
                                             vrbe->super.dst_len,
                                             addr_str),
                      (unsigned)vrbe->super.datagram_size, vrbe->super.tag);
                DEBUG("(%s, %u)\n",
                      gnrc_netif_addr_to_str(vrbe->super.dst,
                                             vrbe->super.dst_len,
                                             addr_str), vrbe->out_tag);
            }
            /* _equal_index() => append intervals of `base`, so they don't get
             * lost. We use append, so we don't need to change base! */
            else if (base->ints != NULL) {
                gnrc_sixlowpan_frag_rb_int_t *tmp = vrbe->super.ints;

                if (tmp != base->ints) {
                    /* base->ints is not already vrbe->super.ints */
                    if (tmp != NULL) {
                        /* iterate before appending and check if `base->ints` is
                         * not already part of list */
                        while (tmp->next != NULL) {
                            if (tmp == base->ints) {
                                tmp = NULL;
                                break;
                            }
                            tmp = tmp->next;
                        }
                        if (tmp != NULL) {
                            tmp->next = base->ints;
                        }
                    }
                    else {
                        vrbe->super.ints = base->ints;
                    }
                }
            }
            break;
        }
    }
#ifdef MODULE_GNRC_SIXLOWPAN_FRAG_STATS
    if (vrbe == NULL) {
        gnrc_sixlowpan_frag_stats_get()->vrb_full++;
    }
#endif
    return vrbe;
}
