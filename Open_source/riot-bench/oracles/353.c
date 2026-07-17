int vfs_bind(int fd, int flags, const vfs_file_ops_t *f_op, void *private_data)
{
    DEBUG("vfs_bind: %d, %d, %p, %p\n", fd, flags, (void*)f_op, private_data);
    if (f_op == NULL) {
        return -EINVAL;
    }
    mutex_lock(&_open_mutex);
    fd = _init_fd(fd, f_op, NULL, flags, private_data);
    mutex_unlock(&_open_mutex);
    if (fd < 0) {
        DEBUG("vfs_bind: _init_fd: ERR %d!\n", fd);
        return fd;
    }
    DEBUG("vfs_bind: bound %d\n", fd);
    return fd;
}
