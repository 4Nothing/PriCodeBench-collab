static U32 OsDynModuleGetLoadSegData(struct DynModuleUnitInfo *unitInfo)
{
    U32 i, ret;
    Elf_Ehdr *elfInfo = (Elf_Ehdr *)(unitInfo->moduleStr);
    Elf_Phdr *phdr = (Elf_Phdr *)(unitInfo->moduleStr + elfInfo->e_phoff);
    for (i = 0; i < elfInfo->e_phnum; i++) {
        if (phdr[i].p_type != PT_LOAD) {
            continue;
        }
        /* 前面申请内存的时候已经初始化0了，这里拷贝的大小使用p_filesz而不是用p_memsz的原因就是memsz-filesz这一块是bss，需要清0 */
        ret = memcpy_s(unitInfo->loadSegMem + phdr[i].p_vaddr - unitInfo->loadSegStartAddr,
            phdr[i].p_filesz, unitInfo->moduleStr + phdr[i].p_offset, phdr[i].p_filesz);
        if (ret != EOK) {
            return OS_MODULE_ERRNO_MEMCOPY;
        }
    }
#ifndef OS_ARCH_X86_64
    os_asm_invalidate_dcache_all();
#endif
    return OS_MODULE_OK;
}
