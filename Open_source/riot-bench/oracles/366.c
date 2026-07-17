int vfs_readdir(vfs_DIR *dirp, vfs_dirent_t *entry)
{
    DEBUG("vfs_readdir: %p, %p\n", (void *)dirp, (void *)entry);
    if ((dirp == NULL) || (entry == NULL)) {
        return -EINVAL;
    }
    if (dirp->d_op != NULL) {
        if (dirp->d_op->readdir != NULL) {
            return dirp->d_op->readdir(dirp, entry);
        }
    }
    return -EINVAL;
}
