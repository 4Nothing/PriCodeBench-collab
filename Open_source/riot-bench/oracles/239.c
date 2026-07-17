int open(const char *name, int flags, int mode)
{
#ifdef MODULE_VFS
    int fd = vfs_open(name, flags, mode);
    if (fd < 0) {
        /* vfs returns negative error codes */
        errno = -fd;
        return -1;
    }
    return fd;
#else
    (void)name;
    (void)flags;
    (void)mode;
    errno = ENODEV;
    return -1;
#endif
}