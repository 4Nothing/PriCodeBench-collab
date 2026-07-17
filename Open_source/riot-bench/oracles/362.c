int vfs_normalize_path(char *buf, const char *path, size_t buflen)
{
    DEBUG("vfs_normalize_path: %p, \"%s\" (%p), %" PRIuSIZE "\n",
          (void *)buf, path, (void *)path, buflen);
    size_t len = 0;
    int npathcomp = 0;
    const char *path_end = path + strlen(path); /* Find the terminating null byte */
    if (len >= buflen) {
        return -ENAMETOOLONG;
    }

    while(path <= path_end) {
        DEBUG("vfs_normalize_path: + %d \"%.*s\" <- \"%s\" (%p)\n",
              npathcomp, (int)len, buf, path, (void *)path);
        if (path[0] == '\0') {
            break;
        }
        while (path[0] == '/') {
            /* skip extra slashes */
            ++path;
        }
        if (path[0] == '.') {
            ++path;
            if (path[0] == '/' || path[0] == '\0') {
                /* skip /./ components */
                DEBUG("vfs_normalize_path: skip .\n");
                continue;
            }
            if (path[0] == '.' && (path[1] == '/' || path[1] == '\0')) {
                DEBUG("vfs_normalize_path: reduce ../\n");
                if (len == 0) {
                    /* outside root */
                    return -EINVAL;
                }
                ++path;
                /* delete the last component of the path */
                while (len > 0 && buf[--len] != '/') {}
                --npathcomp;
                continue;
            }
        }
        buf[len++] = '/';
        if (len >= buflen) {
            return -ENAMETOOLONG;
        }
        if (path[0] == '\0') {
            /* trailing slash in original path, don't increment npathcomp */
            break;
        }
        ++npathcomp;
        /* copy the path component */
        while (len < buflen && path[0] != '/' && path[0] != '\0') {
            buf[len++] = path[0];
            ++path;
        }
        if (len >= buflen) {
            return -ENAMETOOLONG;
        }
    }
    /* special case for "/": (otherwise it will be zero) */
    if (len == 1) {
        npathcomp = 1;
    }
    buf[len] = '\0';
    DEBUG("vfs_normalize_path: = %d, \"%s\"\n", npathcomp, buf);
    return npathcomp;
}
