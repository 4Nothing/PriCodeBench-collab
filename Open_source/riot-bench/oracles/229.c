int mtd_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (out_of_bounds(mtd, 0, addr, count)) {
        return -EOVERFLOW;
    }

    if (mtd->driver->read) {
        return mtd->driver->read(mtd, dest, addr, count);
    }

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    return mtd_read_page(mtd, dest, addr >> page_shift, addr & page_mask, count);
}