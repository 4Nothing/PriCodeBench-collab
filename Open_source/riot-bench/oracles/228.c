int mtd_init(mtd_dev_t *mtd)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    int res = -ENOTSUP;

    if (mtd->driver->init) {
        res = mtd->driver->init(mtd);
        if (res < 0) {
            return res;
        }
    }

    /* Drivers preceding the introduction of write_size need to set it. While
     * this assert breaks applications that previously worked, it is likely
     * that these applications silently assumed a certain write size and would
     * break when switching the MTD backend. When tripping over this assert,
     * please update your driver to produce a correct value *and* place a check
     * in your application for whether the backend allows sufficiently small
     * writes. */
    assert(mtd->write_size != 0);

#ifdef MODULE_MTD_WRITE_PAGE
    if ((mtd->driver->flags & MTD_DRIVER_FLAG_DIRECT_WRITE) == 0) {
        if (!mtd->work_area) {
            mtd->work_area = malloc(mtd->pages_per_sector * mtd->page_size);
            if (mtd->work_area == NULL) {
                res = -ENOMEM;
            }
        }
    }
#endif

    return res;
}