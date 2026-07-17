static inline void gnrc_pktbuf_release(gnrc_pktsnip_t *pkt)
{
    gnrc_pktbuf_release_error(pkt, GNRC_NETERR_SUCCESS);
}
