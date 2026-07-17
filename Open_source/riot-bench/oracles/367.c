int vfs_rename(const char *from_path, const char *to_path)
{
    DEBUG("vfs_rename: \"%s\", \"%s\"\n", from_path, to_path);
    if ((from_path == NULL) || (to_path == NULL)) {
        return -EINVAL;
    }
    const char *rel_from;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, from_path, &rel_from);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_rename: from: no matching mount\n");
        return res;
    }
    if ((mountp->fs->fs_op == NULL) || (mountp->fs->fs_op->rename == NULL)) {
        /* rename not supported */
        DEBUG("vfs_rename: rename not supported by fs!\n");
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        return -EROFS;
    }
    const char *rel_to;
    vfs_mount_t *mountp_to;
    res = _find_mount(&mountp_to, to_path, &rel_to);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_rename: to: no matching mount\n");
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        return res;
    }
    if (mountp_to != mountp) {
        /* The paths are on different file systems */
        DEBUG("vfs_rename: from_path and to_path are on different mounts\n");
        /* remember to decrement the open_files count */
        uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
        assume(before > 0);
        before = atomic_fetch_sub_u16(&mountp_to->open_files, 1);
        assume(before > 0);
        return -EXDEV;
    }
    res = mountp->fs->fs_op->rename(mountp, rel_from, rel_to);
    DEBUG("vfs_rename: rename %p, \"%s\" -> \"%s\"", (void *)mountp, rel_from, rel_to);
    if (res < 0) {
        /* something went wrong during rename */
        DEBUG(": ERR %d!\n", res);
    }
    else {
        DEBUG("\n");
    }
    /* remember to decrement the open_files count */
    uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
    assume(before > 0);
    before = atomic_fetch_sub_u16(&mountp_to->open_files, 1);
    assume(before > 0);
    return res;
}
