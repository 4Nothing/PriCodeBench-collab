int mtd_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t count)
{
    if (!mtd || !mtd->driver) {
        return -ENODEV;
    }

    if (mtd->driver->erase) {
        return mtd->driver->erase(mtd, addr, count);
    }

    uint32_t sector_size = mtd->pages_per_sector * mtd->page_size;

    if (count % sector_size) {
        return -EOVERFLOW;
    }

    if (addr % sector_size) {
        return -EOVERFLOW;
    }

    return mtd_erase_sector(mtd, addr / sector_size, count / sector_size);
}