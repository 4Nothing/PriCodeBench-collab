int sock_urlsplit(const char *url, char *hostport, char *urlpath)
{
    assert(url);
    char *hoststart = _find_hoststart(url);
    if (!hoststart) {
        return -EINVAL;
    }

    char *pathstart = _find_pathstart(hoststart);

    if (hostport) {
        size_t hostlen = pathstart - hoststart;
        /* hostlen must be smaller CONFIG_SOCK_HOSTPORT_MAXLEN to have space for the null
        * terminator */
        if (hostlen > CONFIG_SOCK_HOSTPORT_MAXLEN - 1) {
            return -EOVERFLOW;
        }
        memcpy(hostport, hoststart, hostlen);
        hostport[hostlen] = '\0';
    }

    if (urlpath) {
        size_t pathlen = strlen(pathstart);
        if (pathlen > CONFIG_SOCK_URLPATH_MAXLEN - 1) {
            return -EOVERFLOW;
        }
        memcpy(urlpath, pathstart, pathlen);
        urlpath[pathlen] = '\0';
    }
    return 0;
}
