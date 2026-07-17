int vfs_mount(vfs_mount_t *mountp)
{
    DEBUG("vfs_mount: %p\n", (void *)mountp);
    int ret = check_mount(mountp);
    if (ret < 0) {
        return ret;
    }

    if (mountp->fs->fs_op != NULL) {
        if (mountp->fs->fs_op->mount != NULL) {
            /* yes, a file system driver does not need to implement mount/umount */
            int res = mountp->fs->fs_op->mount(mountp);
            if (res < 0) {
                DEBUG("vfs_mount: error %d\n", res);
                mutex_unlock(&_mount_mutex);
                return res;
            }
        }
    }
    /* Insert last in list. This property is relied on by vfs_iterate_mount_dirs. */
    clist_rpush(&_vfs_mounts_list, &mountp->list_entry);
    mutex_unlock(&_mount_mutex);
    DEBUG("vfs_mount: mount done\n");
    return 0;
}
