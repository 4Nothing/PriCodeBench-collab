static inline int sock_udp_ep_fmt(const sock_udp_ep_t *endpoint,
                                  char *addr_str, uint16_t *port)
{
    return sock_tl_ep_fmt(endpoint, addr_str, port);
}
