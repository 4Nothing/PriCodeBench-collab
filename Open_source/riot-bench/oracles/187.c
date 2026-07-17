gnrc_sixlowpan_frag_vrb_t *gnrc_sixlowpan_frag_vrb_get(
        const uint8_t *src, size_t src_len, unsigned src_tag)
{
    DEBUG("6lo vrb: trying to get entry for (%s, %u)\n",
          gnrc_netif_addr_to_str(src, src_len, addr_str), src_tag);
    assert(src_len != 0);
    for (unsigned i = 0; i < CONFIG_GNRC_SIXLOWPAN_FRAG_VRB_SIZE; i++) {
        gnrc_sixlowpan_frag_vrb_t *vrbe = &_vrb[i];

        if (_equal_index(vrbe, src, src_len, src_tag)) {
            DEBUG("6lo vrb: got VRB to (%s, %u)\n",
                  gnrc_netif_addr_to_str(vrbe->super.dst,
                                         vrbe->super.dst_len,
                                         addr_str), vrbe->out_tag);
            return vrbe;
        }
    }
    DEBUG("6lo vrb: no entry found\n");
    return NULL;
}
