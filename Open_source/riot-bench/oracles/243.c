_READ_WRITE_RETURN_TYPE read(int fd, void *dest, size_t count)
{
#ifdef MODULE_VFS
    int res = vfs_read(fd, dest, count);
    if (res < 0) {
        /* vfs returns negative error codes */
        errno = -res;
        return -1;
    }
    return res;
#else
    if (fd != STDIN_FILENO) {
        errno = ENOTSUP;
        return -1;
    }

    return stdio_read(dest, count);
#endif
}