int vfs_closedir(vfs_DIR *dirp)
{
    DEBUG("vfs_closedir: %p\n", (void *)dirp);
    if (dirp == NULL) {
        return -EINVAL;
    }
    vfs_mount_t *mountp = dirp->mp;
    if (mountp == NULL) {
        return -EBADF;
    }
    int res = 0;
    if (dirp->d_op != NULL) {
        if (dirp->d_op->closedir != NULL) {
            res = dirp->d_op->closedir(dirp);
        }
    }
    memset(dirp, 0, sizeof(*dirp));
    uint16_t before = atomic_fetch_sub_u16(&mountp->open_files, 1);
    assume(before > 0);
    return res;
}
