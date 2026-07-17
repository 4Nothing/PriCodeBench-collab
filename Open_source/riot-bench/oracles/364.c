int vfs_opendir(vfs_DIR *dirp, const char *dirname)
{
    DEBUG("vfs_opendir: %p, \"%s\"\n", (void *)dirp, dirname);
    if ((dirp == NULL) || (dirname == NULL)) {
        return -EINVAL;
    }
    const char *rel_path;
    vfs_mount_t *mountp;
    int res = _find_mount(&mountp, dirname, &rel_path);
    /* _find_mount implicitly increments the open_files count on success */
    if (res < 0) {
        /* No mount point maps to the requested file name */
        DEBUG("vfs_open: no matching mount\n");
        return res;
    }
    if (rel_path[0] == '\0') {
        /* if the trailing slash is missing we will get an empty string back, to
         * be consistent against the file system drivers we give the relative
         * path "/" instead */
        rel_path = "/";
    }
    if (mountp->fs->d_op == NULL) {
        /* file system driver does not support directories */
        return -EINVAL;
    }
    /* initialize dirp */
    memset(dirp, 0, sizeof(*dirp));
    dirp->mp = mountp;
    dirp->d_op = mountp->fs->d_op;
    if (dirp->d_op->opendir != NULL) {
        int res = dirp->d_op->opendir(dirp, rel_path);
        if (res < 0) {
            /* remember to decrement the open_files count */
            uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
            assume(before > 0);
            return res;
        }
    }
    return 0;
}
