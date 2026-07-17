#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_common.h"
#include "func_under_test.c"

static int run_cases(void)
{
    int pass = 0, total = 0;
    U32 ret;

    // 1. no PT_LOAD -> OK
    total++;
    struct DynModuleUnitInfo u1;
    memset(&u1, 0, sizeof(u1));
    uint8_t m1[64] = {0};
    u1.moduleStr = (char *)m1;
    Elf_Ehdr *e1 = (Elf_Ehdr *)m1;
    e1->e_phoff = sizeof(Elf_Ehdr);
    e1->e_phnum = 0;
    ret = OsDynModuleGetLoadSegData(&u1);
    bool ok1 = (ret == OS_MODULE_OK);
    print_case("no PT_LOAD", "OK", ok1 ? "OK" : "fail", ok1);
    pass += ok1;

    // 2. one PT_LOAD with filesz=0 -> OK
    total++;
    struct DynModuleUnitInfo u2;
    memset(&u2, 0, sizeof(u2));
    uint8_t m2[128] = {0};
    u2.moduleStr = (char *)m2;
    Elf_Ehdr *e2 = (Elf_Ehdr *)m2;
    e2->e_phoff = sizeof(Elf_Ehdr);
    e2->e_phnum = 1;
    Elf_Phdr *p2 = (Elf_Phdr *)(m2 + sizeof(Elf_Ehdr));
    p2[0].p_type = PT_LOAD;
    p2[0].p_offset = 64;
    p2[0].p_vaddr = 0x1000;
    p2[0].p_filesz = 0;
    p2[0].p_memsz = 16;
    u2.loadSegStartAddr = 0x1000;
    uint8_t buf2[32];
    u2.loadSegMem = buf2;
    ret = OsDynModuleGetLoadSegData(&u2);
    bool ok2 = (ret == OS_MODULE_OK);
    print_case("one zero-sized", "OK", ok2 ? "OK" : "fail", ok2);
    pass += ok2;

    // 3. two PT_LOAD zero-sized -> OK
    total++;
    struct DynModuleUnitInfo u3;
    memset(&u3, 0, sizeof(u3));
    uint8_t m3[128] = {0};
    u3.moduleStr = (char *)m3;
    Elf_Ehdr *e3 = (Elf_Ehdr *)m3;
    e3->e_phoff = sizeof(Elf_Ehdr);
    e3->e_phnum = 2;
    Elf_Phdr *p3 = (Elf_Phdr *)(m3 + sizeof(Elf_Ehdr));
    p3[0] = p2[0];
    p3[1] = p2[0];
    p3[1].p_vaddr = 0x1008;
    u3.loadSegStartAddr = 0x1000;
    uint8_t buf3[32];
    u3.loadSegMem = buf3;
    ret = OsDynModuleGetLoadSegData(&u3);
    bool ok3 = (ret == OS_MODULE_OK);
    print_case("two zero-sized", "OK", ok3 ? "OK" : "fail", ok3);
    pass += ok3;

    // 4. mixed types with zero-sized PT_LOAD -> OK
    total++;
    struct DynModuleUnitInfo u4;
    memset(&u4, 0, sizeof(u4));
    uint8_t m4[128] = {0};
    u4.moduleStr = (char *)m4;
    Elf_Ehdr *e4 = (Elf_Ehdr *)m4;
    e4->e_phoff = sizeof(Elf_Ehdr);
    e4->e_phnum = 2;
    Elf_Phdr *p4 = (Elf_Phdr *)(m4 + sizeof(Elf_Ehdr));
    p4[0].p_type = PT_NOTE;
    p4[1] = p2[0];
    u4.loadSegStartAddr = 0x1000;
    uint8_t buf4[32];
    u4.loadSegMem = buf4;
    ret = OsDynModuleGetLoadSegData(&u4);
    bool ok4 = (ret == OS_MODULE_OK);
    print_case("mixed zero-sized", "OK", ok4 ? "OK" : "fail", ok4);
    pass += ok4;

    // 5. NULL dest with zero filesz -> memcpy_s returns OK for count=0
    total++;
    struct DynModuleUnitInfo u5;
    memset(&u5, 0, sizeof(u5));
    uint8_t m5[128] = {0};
    u5.moduleStr = (char *)m5;
    Elf_Ehdr *e5 = (Elf_Ehdr *)m5;
    e5->e_phoff = sizeof(Elf_Ehdr);
    e5->e_phnum = 1;
    Elf_Phdr *p5 = (Elf_Phdr *)(m5 + sizeof(Elf_Ehdr));
    p5[0] = p2[0];
    u5.loadSegStartAddr = 0x1000;
    u5.loadSegMem = NULL; // but filesz=0
    ret = OsDynModuleGetLoadSegData(&u5);
    bool ok5 = (ret == OS_MODULE_OK);
    print_case("NULL dest zero-sized", "OK", ok5 ? "OK" : "fail", ok5);
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }
