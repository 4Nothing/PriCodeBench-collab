#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test_common.h"
#include "func_under_test.c"

static void make_valid_header(Elf_Ehdr *eh)
{
    memset(eh, 0, sizeof(*eh));
    eh->e_ident[EI_MAG0] = ELFMAG0;
    eh->e_ident[EI_MAG1] = ELFMAG1;
    eh->e_ident[EI_MAG2] = ELFMAG2;
    eh->e_ident[EI_MAG3] = ELFMAG3;
    eh->e_ident[EI_CLASS] = ELFCLASS64;
    eh->e_ident[EI_DATA] = ELFDATA2LSB;
    eh->e_ident[EI_VERSION] = EV_CURRENT;
    eh->e_ehsize = sizeof(Elf_Ehdr);
    eh->e_shentsize = sizeof(Elf_Shdr);
    eh->e_shnum = 1;
    eh->e_machine = EM_X86_64;
    eh->e_type = ET_DYN;
}

static int run_cases(void)
{
    int pass = 0, total = 0;
    U32 ret;
    Elf_Ehdr eh;

    // 1. bad magic
    total++;
    memset(&eh, 0, sizeof(eh));
    eh.e_ident[EI_MAG0] = 'B';
    ret = OsDynModuleCheckElf(&eh);
    bool ok1 = (ret == OS_MODULE_ERRNO_ELF_HEAD_MAGIC);
    print_case("bad magic", "ERR_MAGIC", ok1 ? "ERR_MAGIC" : "other", ok1);
    pass += ok1;

    // 2. bad class
    total++;
    make_valid_header(&eh);
    eh.e_ident[EI_CLASS] = 0;
    ret = OsDynModuleCheckElf(&eh);
    bool ok2 = (ret == OS_MODULE_ERRNO_ELF_HEAD_CLASS);
    print_case("bad class", "ERR_CLASS", ok2 ? "ERR_CLASS" : "other", ok2);
    pass += ok2;

    // 3. wrong ehsize
    total++;
    make_valid_header(&eh);
    eh.e_ehsize = 0;
    ret = OsDynModuleCheckElf(&eh);
    bool ok3 = (ret == OS_MODULE_ERRNO_ELF_HEAD_LEN);
    print_case("bad ehsize", "ERR_LEN", ok3 ? "ERR_LEN" : "other", ok3);
    pass += ok3;

    // 4. wrong shentsize
    total++;
    make_valid_header(&eh);
    eh.e_shentsize = 0;
    ret = OsDynModuleCheckElf(&eh);
    bool ok4 = (ret == OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE);
    print_case("bad shentsize", "ERR_SHENT_SIZE", ok4 ? "ERR_SHENT_SIZE" : "other", ok4);
    pass += ok4;

    // 5. zero shnum
    total++;
    make_valid_header(&eh);
    eh.e_shnum = 0;
    ret = OsDynModuleCheckElf(&eh);
    bool ok5 = (ret == OS_MODULE_ERRNO_ELF_HEAD_SHNUM);
    print_case("zero shnum", "ERR_SHNUM", ok5 ? "ERR_SHNUM" : "other", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }
