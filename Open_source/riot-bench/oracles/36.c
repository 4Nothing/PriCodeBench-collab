int close(int fd)
{
#ifdef MODULE_VFS
    int res = vfs_close(fd);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    errno = ENOTSUP;
    return -1;
#endif
}