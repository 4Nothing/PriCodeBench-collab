void gnrc_pktbuf_hold(gnrc_pktsnip_t *pkt, unsigned int num)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    while (pkt) {
        assert(pkt->users + num <= 0xff);
        pkt->users += num;
        pkt = pkt->next;
    }
    mutex_unlock(&gnrc_pktbuf_mutex);
}