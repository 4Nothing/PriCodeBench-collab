ssize_t vfs_read(int fd, void *dest, size_t count)
{
    DEBUG("vfs_read: %d, %p, %" PRIuSIZE "\n", fd, dest, count);
    vfs_file_t *filp = NULL;

    int res = _prep_read(fd, dest, &filp);
    if (res) {
        DEBUG("vfs_read: can't open file - %d\n", res);
        return res;
    }

    return filp->f_op->read(filp, dest, count);
}
