int vfs_open(const char *name, int flags, mode_t mode)
{
    DEBUG("vfs_open: \"%s\", 0x%x, 0%03lo\n", name, flags, (long unsigned int)mode);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_open: no matching mount\n");
        return res;
    }
    mutex_lock(&_open_mutex);
    int fd = _init_fd(VFS_ANY_FD, mountp->fs->f_op, mountp, flags, NULL);
    mutex_unlock(&_open_mutex);
    if (fd < 0) {
        DEBUG("vfs_open: _init_fd: ERR %d!\n", fd);
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        return fd;
    }
    vfs_file_t *filp = &_vfs_open_files[fd];
    if (filp->f_op->open != NULL) {
        res = filp->f_op->open(filp, rel_path, flags, mode);
        if (res < 0) {
            /* something went wrong during open */
            DEBUG("vfs_open: open: ERR %d!\n", res);
            /* clean up */
            _free_fd(fd);
            return res;
        }
    }
    DEBUG("vfs_open: opened %d\n", fd);
    return fd;
}
