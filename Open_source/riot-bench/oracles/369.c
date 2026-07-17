int vfs_stat(const char *restrict path, struct stat *restrict buf)
{
    DEBUG("vfs_stat: \"%s\", %p\n", path, (void *)buf);
    if (path == NULL || buf == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, path, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_stat: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->stat == NULL)) {
        /* stat not supported */
        DEBUG("vfs_stat: stat not supported by fs!\n");
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        return -EPERM;
    }
    memset(buf, 0, sizeof(*buf));
    res = mountp->fs->fs_op->stat(mountp, rel_path, buf);
    /* remember to decrement the open_files count */
    uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
    assume(before > 0);
    return res;
}
