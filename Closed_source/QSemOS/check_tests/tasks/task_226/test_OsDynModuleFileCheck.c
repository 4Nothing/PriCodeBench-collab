#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "test_common.h"
#include "func_under_test.c"

static int run_cases(void)
{
    int pass = 0, total = 0;

    // Test OsDynModuleCheckElf function from func_under_test.c
    total++;
    Elf_Ehdr validElf = {0};
    validElf.e_ident[EI_MAG0] = ELFMAG0;
    validElf.e_ident[EI_MAG1] = ELFMAG1;
    validElf.e_ident[EI_MAG2] = ELFMAG2;
    validElf.e_ident[EI_MAG3] = ELFMAG3;
    validElf.e_ident[EI_CLASS] = ELFCLASS64;
    validElf.e_ehsize = sizeof(Elf_Ehdr);
    validElf.e_shentsize = sizeof(Elf_Shdr);
    validElf.e_shnum = 1;
    validElf.e_machine = EM_AARCH64;
    validElf.e_type = ET_DYN;
    
    U32 ret = OsDynModuleCheckElf(&validElf);
    bool ok1 = (ret == OS_MODULE_OK);
    print_case("valid ELF", "OS_MODULE_OK", ok1 ? "OS_MODULE_OK" : "fail", ok1);
    pass += ok1;

    // Test invalid magic
    total++;
    Elf_Ehdr badMagic = validElf;
    badMagic.e_ident[EI_MAG0] = 0x00;
    ret = OsDynModuleCheckElf(&badMagic);
    bool ok2 = (ret == OS_MODULE_ERRNO_ELF_HEAD_MAGIC);
    print_case("bad magic", "ELF_HEAD_MAGIC", ok2 ? "ELF_HEAD_MAGIC" : "fail", ok2);
    pass += ok2;

    // Test invalid class
    total++;
    Elf_Ehdr badClass = validElf;
    badClass.e_ident[EI_CLASS] = 0xFF;
    ret = OsDynModuleCheckElf(&badClass);
    bool ok3 = (ret == OS_MODULE_ERRNO_ELF_HEAD_CLASS);
    print_case("bad class", "ELF_HEAD_CLASS", ok3 ? "ELF_HEAD_CLASS" : "fail", ok3);
    pass += ok3;

    // Test zero section count
    total++;
    Elf_Ehdr noSections = validElf;
    noSections.e_shnum = 0;
    ret = OsDynModuleCheckElf(&noSections);
    bool ok4 = (ret == OS_MODULE_ERRNO_ELF_HEAD_SHNUM);
    print_case("zero sections", "ELF_HEAD_SHNUM", ok4 ? "ELF_HEAD_SHNUM" : "fail", ok4);
    pass += ok4;

    // Test unsupported machine
    total++;
    Elf_Ehdr badMachine = validElf;
    badMachine.e_machine = 0xFF;
    ret = OsDynModuleCheckElf(&badMachine);
    bool ok5 = (ret == OS_MODULE_ERRNO_ELF_HEAD_MACHINE);
    print_case("bad machine", "ELF_HEAD_MACHINE", ok5 ? "ELF_HEAD_MACHINE" : "fail", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }
