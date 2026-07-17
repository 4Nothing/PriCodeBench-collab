static U32 OsDynModuleCheckElf(Elf_Ehdr *elfInfo)
{
    /* 魔术字校验 */
    if ((elfInfo->e_ident[EI_MAG0] != ELFMAG0) || (elfInfo->e_ident[EI_MAG1] != ELFMAG1) ||
        (elfInfo->e_ident[EI_MAG2] != ELFMAG2) || (elfInfo->e_ident[EI_MAG3] != ELFMAG3)) {
        return OS_MODULE_ERRNO_ELF_HEAD_MAGIC;
    }
    /* ELF文件类型校验 */
    if ((elfInfo->e_ident[EI_CLASS] != ELFCLASS32) && (elfInfo->e_ident[EI_CLASS] != ELFCLASS64)) {
        return OS_MODULE_ERRNO_ELF_HEAD_CLASS;
    }
    /* ELF头文件大小校验 */
    if (elfInfo->e_ehsize != sizeof(Elf_Ehdr)) {
        return OS_MODULE_ERRNO_ELF_HEAD_LEN;
    }
    /* 判断节区表大小 */
    if (elfInfo->e_shentsize != sizeof(Elf_Shdr)) {
        return OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE;
    }
    /* 检查节区个数 */
    if (elfInfo->e_shnum == 0) {
        return OS_MODULE_ERRNO_ELF_HEAD_SHNUM;
    }
    /* 当前仅支持x86和ARM64架构 */
    if ((elfInfo->e_machine != EM_X86_64) && (elfInfo->e_machine != EM_AARCH64)) {
        return OS_MODULE_ERRNO_ELF_HEAD_MACHINE;
    }
    /* 当前仅支持DYN类型 */
    if (elfInfo->e_type != ET_DYN) {
        return OS_MODULE_ERRNO_ELF_HEAD_TYPE;
    }
    return OS_MODULE_OK;
}
