int vfs_umount(vfs_mount_t *mountp, bool force)
{
    DEBUG("vfs_umount: %p\n", (void *)mountp);
    int ret = check_mount(mountp);
    switch (ret) {
    case 0:
        DEBUG("vfs_umount: not mounted\n");
        mutex_unlock(&_mount_mutex);
        return -EINVAL;
    case -EBUSY:
        /* -EBUSY returned when fs is mounted, just continue */
        break;
    default:
        DEBUG("vfs_umount: invalid fs\n");
        return -EINVAL;
    }
    DEBUG("vfs_umount: -> \"%s\" open=%u\n", mountp->mount_point,
          (unsigned)atomic_load_u16(&mountp->open_files));
    if (atomic_load_u16(&mountp->open_files) > 0 && !force) {
        mutex_unlock(&_mount_mutex);
        return -EBUSY;
    }
    if (mountp->fs->fs_op != NULL) {
        if (mountp->fs->fs_op->umount != NULL) {
            int res = mountp->fs->fs_op->umount(mountp);
            if (res < 0) {
                /* umount failed */
                DEBUG("vfs_umount: ERR %d!\n", res);
                mutex_unlock(&_mount_mutex);
                return res;
            }
        }
    }
    /* find mountp in the list and remove it */
    clist_node_t *node = clist_remove(&_vfs_mounts_list, &mountp->list_entry);
    if (node == NULL) {
        /* not found */
        DEBUG("vfs_umount: ERR not mounted!\n");
        mutex_unlock(&_mount_mutex);
        return -EINVAL;
    }
    mutex_unlock(&_mount_mutex);
    return 0;
}
