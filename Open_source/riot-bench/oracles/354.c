int vfs_close(int fd)
{
    DEBUG("vfs_close: %d\n", fd);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->close != NULL) {
        /* We will invalidate the fd regardless of the outcome of the file
         * system driver close() call below */
        res = filp->f_op->close(filp);
    }
    _free_fd(fd);
    return res;
}
