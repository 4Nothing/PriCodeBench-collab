int vfs_fstatvfs(int fd, struct statvfs *buf)
{
    DEBUG("vfs_fstatvfs: %d, %p\n", fd, (void *)buf);
    if (buf == NULL) {
        return -EFAULT;
    }
    int res = _fd_is_valid(fd);
    if (res < 0) {
        return res;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    memset(buf, 0, sizeof(*buf));
    if (filp->mp->fs->fs_op->statvfs == NULL) {
        /* file system driver does not implement statvfs() */
        return -EINVAL;
    }
    return filp->mp->fs->fs_op->statvfs(filp->mp, "/", buf);
}
