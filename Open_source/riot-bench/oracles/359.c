off_t vfs_lseek(int fd, off_t off, int whence)
{
    DEBUG("vfs_lseek: %d, %ld, %d\n", fd, (long)off, whence);
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->lseek == NULL) {
        /* driver does not implement lseek() */
        /* default seek functionality is naive */
        switch (whence) {
            case SEEK_SET:
                break;
            case SEEK_CUR:
                off += filp->pos;
                break;
            case SEEK_END:
                /* we could use fstat here, but most file system drivers will
                 * likely already implement lseek in a more efficient fashion */
                return -EINVAL;
            default:
                return -EINVAL;
        }
        if (off < 0) {
            /* the resulting file offset would be negative */
            return -EINVAL;
        }
        filp->pos = off;

        return off;
    }
    return filp->f_op->lseek(filp, off, whence);
}
