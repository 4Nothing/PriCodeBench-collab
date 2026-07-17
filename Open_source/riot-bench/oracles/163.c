gnrc_pktsnip_t *gnrc_pktbuf_add(gnrc_pktsnip_t *next, const void *data, size_t size,
                                gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt;

    if (size > CONFIG_GNRC_PKTBUF_SIZE) {
        DEBUG("pktbuf: size (%" PRIuSIZE ") > CONFIG_GNRC_PKTBUF_SIZE (%u)\n",
              size, CONFIG_GNRC_PKTBUF_SIZE);
        return NULL;
    }
    mutex_lock(&gnrc_pktbuf_mutex);
    pkt = _create_snip(next, data, size, type);
    mutex_unlock(&gnrc_pktbuf_mutex);
    return pkt;
}