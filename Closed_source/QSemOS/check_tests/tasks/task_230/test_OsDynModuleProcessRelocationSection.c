#include "support.h"
#include "prt_dynmodule_external.h"

#include "func_under_test.c"

int main(void)
{
    int pass = 0, total = 0;

    // Case: Valid ELF header with e_shoff=0 (no sections) -> returns OK
    struct DynModuleUnitInfo testUnit = {0};
    char elfModule[64] = {0};
    // ELF magic number at offset 0
    elfModule[0] = 0x7f; elfModule[1] = 'E'; elfModule[2] = 'L'; elfModule[3] = 'F';
    // e_shoff = 0 at offset 40 (meaning no section header table)
    // e_shnum = 0 at offset 60 (meaning 0 section headers)
    testUnit.moduleStr = elfModule;
    U32 r = OsDynModuleProcessRelocationSection(&testUnit);
    print_case_u32("ELF no sections", OS_MODULE_OK, r, &pass, &total);

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}
