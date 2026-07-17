static U32 OsDynModuleAllocMemForLoadSeg(struct DynModuleUnitInfo *unitInfo)
{
    U32 i;
    Elf_Ehdr *elfInfo = (Elf_Ehdr *)(unitInfo->moduleStr);
    Elf_Phdr *phdr = (Elf_Phdr *)(unitInfo->moduleStr + elfInfo->e_phoff);
    bool flag = false;
    for (i = 0; i < elfInfo->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }
        if (phdr[i].p_filesz > phdr[i].p_memsz) {
            return OS_MODULE_ERRNO_PTLOAD_SIZE;
        }
        if (!flag) {
            unitInfo->loadSegStartAddr = phdr[i].p_vaddr;
            flag = true;
        }
        unitInfo->loadSegEndAddr = phdr[i].p_vaddr + phdr[i].p_memsz;
    }
    U32 size = unitInfo->loadSegEndAddr - unitInfo->loadSegStartAddr;
    if (size == 0) {
        return OS_MODULE_ERRNO_PTLOAD_SIZE;
    }
    size = ALIGN(size, OS_MODULE_ALIGN_LEN);
    unitInfo->loadSegMem = (uint8_t *)PRT_MemAllocAlign(OS_MID_DYNAMIC, OS_MEM_DEFAULT_FSC_PT, size, MEM_ADDR_ALIGN_4K);
    if (unitInfo->loadSegMem == NULL) {
        return OS_MODULE_ERRNO_MEMORY_ALLOC;
    }
    (void)memset_s(unitInfo->loadSegMem, size, 0, size);
    return OS_MODULE_OK;
}
