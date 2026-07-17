int gnrc_udp_calc_csum(gnrc_pktsnip_t *hdr, gnrc_pktsnip_t *pseudo_hdr)
{
    uint16_t csum;

    if ((hdr == NULL) || (pseudo_hdr == NULL)) {
        return -EFAULT;
    }
    if (hdr->type != GNRC_NETTYPE_UDP) {
        return -EBADMSG;
    }

    csum = _calc_csum(hdr, pseudo_hdr, hdr->next);
    if (csum == 0) {
        return -ENOENT;
    }
    ((udp_hdr_t *)hdr->data)->checksum = byteorder_htons(csum);
    return 0;
}
