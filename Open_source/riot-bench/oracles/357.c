int vfs_fstat(int fd, struct stat *buf)
{
    DEBUG_NOT_STDOUT(fd, "vfs_fstat: %d, %p\n", fd, (void *)buf);
    if (buf == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->fstat == NULL) {
        /* driver does not implement fstat() */
        return -EINVAL;
    }
    memset(buf, 0, sizeof(*buf));
    return filp->f_op->fstat(filp, buf);
}
