int mtd_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    /* page size is always a power of two */
    const uint32_t page_shift = bitarithm_msb(mtd->page_size);
    const uint32_t page_mask = mtd->page_size - 1;

    return mtd_write_page_raw(mtd, src, addr >> page_shift, addr & page_mask, count);
}