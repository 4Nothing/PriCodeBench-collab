const char *netopt2str(netopt_t opt)
{
    if (opt >= NETOPT_NUMOF) {
        return "unknown";
    }
    else {
        const char *tmp = _netopt_strmap[opt];
        return tmp ? tmp : "unknown";
    }
}