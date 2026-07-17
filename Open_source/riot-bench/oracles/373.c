ssize_t vfs_write(int fd, const void *src, size_t count)
{
    DEBUG_NOT_STDOUT(fd, "vfs_write: %d, %p, %" PRIuSIZE "\n", fd, src, count);
    if (src == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (((filp->flags & O_ACCMODE) != O_WRONLY) & ((filp->flags & O_ACCMODE) != O_RDWR)) {
        /* File not open for writing */
        return -EBADF;
    }
    if (filp->f_op->write == NULL) {
        /* driver does not implement write() */
        return -EINVAL;
    }
    return filp->f_op->write(filp, src, count);
}
