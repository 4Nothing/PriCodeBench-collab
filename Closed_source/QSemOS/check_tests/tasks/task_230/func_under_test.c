static U32 OsDynModuleProcessRelocationSection(struct DynModuleUnitInfo *unitInfo)
{
    U32 i, j, ret;
    Elf_Ehdr *elfInfo = (Elf_Ehdr *)(unitInfo->moduleStr);
    Elf_Shdr *shdr = (Elf_Shdr *)(unitInfo->moduleStr + elfInfo->e_shoff);
    uintptr_t reloc;
    for (i = 0; i < elfInfo->e_shnum; i++) {
        if ((shdr[i].sh_type != SHT_RELA) && (shdr[i].sh_type != SHT_REL)) {
            continue;
        }
        /* 获取要处理的节区表.rela.dyn或.rela.plt */
        reloc = (uintptr_t)(unitInfo->moduleStr + shdr[i].sh_offset);
        /* 通过sh_link获取到符号表节区.dynsym */
        Elf_Sym *symTab = (Elf_Sym *)(unitInfo->moduleStr + shdr[shdr[i].sh_link].sh_offset);
        /* 通过节区.dynsym的sh_link获取到符号名字字符串表节区.dynstr */
        uint8_t *strTab = (uint8_t *)(unitInfo->moduleStr + shdr[shdr[shdr[i].sh_link].sh_link].sh_offset);
        for (j = 0; j < shdr[i].sh_size / shdr[i].sh_entsize; j++, reloc += shdr[i].sh_entsize) {
            /* 从重定向表里获取每一个重定向表项reloc */
            ret = OsDynModuleRelocatePrepare(unitInfo, reloc, symTab, strTab, &shdr[i]);
            if (ret != OS_MODULE_OK) {
                return ret;
            }
        }
    }
#ifndef OS_ARCH_X86_64
    os_asm_invalidate_dcache_all();
    os_asm_invalidate_icache_all();
#endif
    return OS_MODULE_OK;
}
