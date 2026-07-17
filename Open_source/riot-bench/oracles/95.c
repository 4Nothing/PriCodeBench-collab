int fcntl(int fd, int cmd, int arg)
{
#ifdef MODULE_VFS
    int res = vfs_fcntl(fd, cmd, arg);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    (void)fd;
    (void)cmd;
    (void)arg;
    errno = ENOTSUP;
    return -1;
#endif
}