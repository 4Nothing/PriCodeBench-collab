int vfs_fcntl(int fd, int cmd, int arg)
{
    DEBUG("vfs_fcntl: %d, %d, %d\n", fd, cmd, arg);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    /* The default fcntl implementation below only allows querying flags,
     * any other command requires insight into the file system driver */
    switch (cmd) {
        case F_GETFL:
            /* Get file flags */
            DEBUG("vfs_fcntl: GETFL: %d\n", filp->flags);
            return filp->flags;
        default:
            break;
    }
    /* pass on to file system driver */
    if (filp->f_op->fcntl != NULL) {
        return filp->f_op->fcntl(filp, cmd, arg);
    }
    return -EINVAL;
}
