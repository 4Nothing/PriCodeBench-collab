static inline ssize_t coap_opt_add_block1_control(coap_pkt_t *pkt, coap_block1_t *block) {
    return coap_opt_add_uint(pkt, COAP_OPT_BLOCK1,
                             (block->blknum << 4) | block->szx | (block->more ? 0x8 : 0));
}
