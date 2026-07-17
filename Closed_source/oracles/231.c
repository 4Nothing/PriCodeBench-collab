static U32 OsDynModuleRelocatePrepare(struct DynModuleUnitInfo *unitInfo, uintptr_t reloc,
    Elf_Sym *symTab, uint8_t *strTab, Elf_Shdr *shdr)
{
    U32 ret;
    Elf_Addr offset;
    Elf_Sym *sym;
    Elf_Sword addend = 0;
    Elf_Addr relocAddr;
    Elf_Addr symAddr;
    Elf_Word type;
    struct OsDynModuleRelocInfo relocInfo = {0};
    if (shdr->sh_type == SHT_RELA) {
        Elf_Rela *rela = (Elf_Rela *)(reloc);
        offset = rela->r_offset;
        sym = symTab + (uintptr_t)ELF_R_SYM(rela->r_info); /* 获取重定位符号 */
        type = ELF_R_TYPE(rela->r_info);
    } else {
        Elf_Rel *rel = (Elf_Rel *)(reloc);
        offset = rel->r_offset;
        sym = symTab + (uintptr_t)ELF_R_SYM(rel->r_info);
        type = ELF_R_TYPE(rel->r_info);
    }
    relocAddr = (Elf_Addr)(unitInfo->loadSegMem + offset - unitInfo->loadSegStartAddr);
    if ((sym->st_shndx != SHT_NULL) || (ELF_ST_BIND(sym->st_info) == STB_LOCAL)) {
        symAddr = (Elf_Addr)(unitInfo->loadSegMem + sym->st_value - unitInfo->loadSegStartAddr);
    } else {
        symAddr = OsDynModuleFindSymFromOs(strTab + sym->st_name);
    }
    relocInfo.reloc = reloc;
    relocInfo.relocType = type;
    relocInfo.shType = shdr->sh_type;
    relocInfo.symAddr = symAddr;
    ret = OsDynModuleRelocate(relocAddr, relocInfo);
    if (ret != 0) {
        return ret;
    }
    return OS_MODULE_OK;
}
