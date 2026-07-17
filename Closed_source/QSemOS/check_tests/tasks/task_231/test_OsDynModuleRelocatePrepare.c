#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "test_common.h"
#include "test_shims.h"
#include "func_under_test.c"

static int run_cases(void)
{
    int pass = 0, total = 0;
    U32 ret;
    struct DynModuleUnitInfo u;
    memset(&u, 0, sizeof(u));
    uint8_t seg[64] = {0};
    u.loadSegMem = seg;
    u.loadSegStartAddr = 0x1000;
    Elf_Sym symtab[2];
    memset(symtab, 0, sizeof(symtab));
    uint8_t strtab[16];
    memset(strtab, 0, sizeof(strtab));
    strcpy((char *)strtab, "sym");
    Elf_Shdr sh;
    memset(&sh, 0, sizeof(sh));

    // 1. REL with local symbol
    total++;
    Elf_Rel rel1;
    memset(&rel1, 0, sizeof(rel1));
    rel1.r_offset = 0x1000;
    symtab[0].st_shndx = 1;
    symtab[0].st_value = 0x1000;
    sh.sh_type = SHT_REL;
    ret = OsDynModuleRelocatePrepare(&u, (uintptr_t)&rel1, symtab, (uint8_t *)strtab, &sh);
    bool ok1 = (ret == OS_MODULE_OK);
    print_case("REL local", "OK", ok1 ? "OK" : "fail", ok1);
    pass += ok1;

    // 2. REL with global symbol (lookup from OS -> 0)
    total++;
    Elf_Rel rel2;
    memset(&rel2, 0, sizeof(rel2));
    rel2.r_offset = 0x1004;
    symtab[1].st_shndx = SHT_NULL;
    symtab[1].st_info = ELF_ST_INFO(STB_GLOBAL, STT_FUNC);
    symtab[1].st_name = 0;
    rel2.r_info = ELF_R_INFO(1, 1);
    ret = OsDynModuleRelocatePrepare(&u, (uintptr_t)&rel2, symtab, (uint8_t *)strtab, &sh);
    bool ok2 = (ret == OS_MODULE_OK);
    print_case("REL global", "OK", ok2 ? "OK" : "fail", ok2);
    pass += ok2;

    // 3. RELA with local symbol
    total++;
    Elf_Rela rela1;
    memset(&rela1, 0, sizeof(rela1));
    rela1.r_offset = 0x1008;
    symtab[0].st_shndx = 1;
    symtab[0].st_value = 0x1000;
    sh.sh_type = SHT_RELA;
    ret = OsDynModuleRelocatePrepare(&u, (uintptr_t)&rela1, symtab, (uint8_t *)strtab, &sh);
    bool ok3 = (ret == OS_MODULE_OK);
    print_case("RELA local", "OK", ok3 ? "OK" : "fail", ok3);
    pass += ok3;

    // 4. RELA with global symbol
    total++;
    Elf_Rela rela2;
    memset(&rela2, 0, sizeof(rela2));
    rela2.r_offset = 0x100C;
    symtab[1].st_shndx = SHT_NULL;
    symtab[1].st_info = ELF_ST_INFO(STB_GLOBAL, STT_FUNC);
    symtab[1].st_name = 0;
    rela2.r_info = ELF_R_INFO(1, 1);
    ret = OsDynModuleRelocatePrepare(&u, (uintptr_t)&rela2, symtab, (uint8_t *)strtab, &sh);
    bool ok4 = (ret == OS_MODULE_OK);
    print_case("RELA global", "OK", ok4 ? "OK" : "fail", ok4);
    pass += ok4;

    // 5. offset maps within buffer
    total++;
    Elf_Rel rel3;
    memset(&rel3, 0, sizeof(rel3));
    rel3.r_offset = 0x1000 + sizeof(seg) - 1;
    symtab[0].st_shndx = 1;
    symtab[0].st_value = 0x1000;
    sh.sh_type = SHT_REL;
    ret = OsDynModuleRelocatePrepare(&u, (uintptr_t)&rel3, symtab, (uint8_t *)strtab, &sh);
    bool ok5 = (ret == OS_MODULE_OK);
    print_case("REL edge offset", "OK", ok5 ? "OK" : "fail", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }
