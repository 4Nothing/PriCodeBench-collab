int vfs_mkdir(const char *name, mode_t mode)
{
    DEBUG("vfs_mkdir: \"%s\", 0%03lo\n", name, (long unsigned int)mode);
    if (name == NULL) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res;
    res = _find_mount(&mountp, name, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_mkdir: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->mkdir == NULL)) {
        /* mkdir not supported */
        DEBUG("vfs_mkdir: mkdir not supported by fs!\n");
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        return -EROFS;
    }
    res = mountp->fs->fs_op->mkdir(mountp, rel_path, mode);
    DEBUG("vfs_mkdir: mkdir %p, \"%s\"", (void *)mountp, rel_path);
    if (res < 0) {
        /* something went wrong during mkdir */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
    assume(before > 0);
    return res;
}
