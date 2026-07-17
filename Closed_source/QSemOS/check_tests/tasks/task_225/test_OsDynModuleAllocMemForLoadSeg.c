#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "test_common.h"
#include "func_under_test.c"

static void setup_phdr(uint8_t *buf, size_t buflen, Elf_Ehdr *eh, Elf_Phdr *ph, int phnum)
{
    memset(eh, 0, sizeof(*eh));
    eh->e_phoff = sizeof(Elf_Ehdr);
    eh->e_phnum = phnum;
    (void)buf;
    (void)buflen;
    (void)ph;
}

static int run_cases(void)
{
    int pass = 0, total = 0;
    U32 ret;
    struct DynModuleUnitInfo u = {0};
    uint8_t mem[512] = {0};
    u.moduleStr = (char *)mem;
    Elf_Ehdr *eh = (Elf_Ehdr *)mem;
    Elf_Phdr *ph = (Elf_Phdr *)(mem + sizeof(Elf_Ehdr));

    // 1. no PT_LOAD -> size 0 -> PTLOAD_SIZE error
    total++;
    setup_phdr(mem, sizeof(mem), eh, ph, 1);
    ph[0].p_type = PT_NOTE;
    ret = OsDynModuleAllocMemForLoadSeg(&u);
    bool ok1 = (ret == OS_MODULE_ERRNO_PTLOAD_SIZE);
    print_case("no PT_LOAD", "ERR_PTLOAD_SIZE", ok1 ? "ERR_PTLOAD_SIZE" : "other", ok1);
    pass += ok1;

    // 2. filesz > memsz -> error
    total++;
    memset(mem, 0, sizeof(mem));
    eh = (Elf_Ehdr *)mem;
    ph = (Elf_Phdr *)(mem + sizeof(Elf_Ehdr));
    setup_phdr(mem, sizeof(mem), eh, ph, 1);
    ph[0].p_type = PT_LOAD;
    ph[0].p_vaddr = 0x1000;
    ph[0].p_filesz = 16;
    ph[0].p_memsz = 8;
    ret = OsDynModuleAllocMemForLoadSeg(&u);
    bool ok2 = (ret == OS_MODULE_ERRNO_PTLOAD_SIZE);
    print_case("filesz>memsz", "ERR_PTLOAD_SIZE", ok2 ? "ERR_PTLOAD_SIZE" : "other", ok2);
    pass += ok2;

    // 3. single PT_LOAD ok
    total++;
    memset(mem, 0, sizeof(mem));
    eh = (Elf_Ehdr *)mem;
    ph = (Elf_Phdr *)(mem + sizeof(Elf_Ehdr));
    setup_phdr(mem, sizeof(mem), eh, ph, 1);
    ph[0].p_type = PT_LOAD;
    ph[0].p_vaddr = 0x2000;
    ph[0].p_filesz = 8;
    ph[0].p_memsz = 16;
    ret = OsDynModuleAllocMemForLoadSeg(&u);
    bool ok3 = (ret == OS_MODULE_OK) && (u.loadSegMem != NULL) && (u.loadSegEndAddr > u.loadSegStartAddr);
    print_case("single PT_LOAD", "OK & mem alloc", ok3 ? "OK & alloc" : "fail", ok3);
    if (u.loadSegMem)
    {
        PRT_MemFree(OS_MID_DYNAMIC, u.loadSegMem);
        u.loadSegMem = NULL;
    }
    pass += ok3;

    // 4. two PT_LOAD segments contiguous
    total++;
    memset(mem, 0, sizeof(mem));
    eh = (Elf_Ehdr *)mem;
    ph = (Elf_Phdr *)(mem + sizeof(Elf_Ehdr));
    setup_phdr(mem, sizeof(mem), eh, ph, 2);
    ph[0].p_type = PT_LOAD;
    ph[0].p_vaddr = 0x3000;
    ph[0].p_filesz = 4;
    ph[0].p_memsz = 8;
    ph[1].p_type = PT_LOAD;
    ph[1].p_vaddr = 0x3000 + 0x100;
    ph[1].p_filesz = 4;
    ph[1].p_memsz = 8;
    ret = OsDynModuleAllocMemForLoadSeg(&u);
    bool ok4 = (ret == OS_MODULE_OK) && (u.loadSegMem != NULL);
    print_case("two PT_LOAD", "OK", ok4 ? "OK" : "fail", ok4);
    if (u.loadSegMem)
    {
        PRT_MemFree(OS_MID_DYNAMIC, u.loadSegMem);
        u.loadSegMem = NULL;
    }
    pass += ok4;

    // 5. mixed PT types, first PT_LOAD determines start
    total++;
    memset(mem, 0, sizeof(mem));
    eh = (Elf_Ehdr *)mem;
    ph = (Elf_Phdr *)(mem + sizeof(Elf_Ehdr));
    setup_phdr(mem, sizeof(mem), eh, ph, 3);
    ph[0].p_type = PT_NOTE;
    ph[1].p_type = PT_LOAD;
    ph[1].p_vaddr = 0x4000;
    ph[1].p_filesz = 4;
    ph[1].p_memsz = 8;
    ph[2].p_type = PT_LOAD;
    ph[2].p_vaddr = 0x4500;
    ph[2].p_filesz = 4;
    ph[2].p_memsz = 8;
    ret = OsDynModuleAllocMemForLoadSeg(&u);
    bool ok5 = (ret == OS_MODULE_OK) && (u.loadSegStartAddr == 0x4000);
    print_case("mixed types", "OK & start 0x4000", ok5 ? "OK & start 0x4000" : "fail", ok5);
    if (u.loadSegMem)
    {
        PRT_MemFree(OS_MID_DYNAMIC, u.loadSegMem);
        u.loadSegMem = NULL;
    }
    pass += ok5;

    print_pass_rate(pass, total);
    return (pass == total) ? 0 : 1;
}

int main(void) { return run_cases(); }
