const char *sock_urlpath(const char *url)
{
    assert(url);
    char *hoststart = _find_hoststart(url);
    if (!hoststart) {
        return NULL;
    }

    return _find_pathstart(hoststart);
}
