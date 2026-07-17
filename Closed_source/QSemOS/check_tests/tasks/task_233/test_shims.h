#pragma once
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

/* Include auto_stub.h first to get all type definitions */
#include "auto_stub.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Only add additional shims that are NOT in auto_stub.h */
/* auto_stub.h already provides: */
/*   - All basic types (U8, U16, U32, S32, etc.) */
/*   - ELF types (Elf_Ehdr, Elf_Phdr, etc.) */
/*   - DynModuleSymTab, DynModuleUnitInfo, OsDynModuleRelocInfo structs */
/*   - MODULE_UNIT_FREE, MODULE_UNIT_INIT, MODULE_UNIT_ACTIVE enums (as ModuleUnitSate) */
/*   - OS_MODULE_OK and error codes */
/*   - PRT_MemAlloc, PRT_MemFree, PRT_MemAllocAlign (with U32 params) */
/*   - OsModuleIntLock, OsModuleIntUnlock */
/*   - OsDynModuleFindSymFromOs, OsDynModuleRelocate */

#ifndef TEST_SHIMS_EXTRA
#define TEST_SHIMS_EXTRA

/* Additional memory allocation shim that uses posix_memalign */
static inline void *PRT_MemAllocAlignExtra(int mid, int pt, size_t size, size_t align)
{
    (void)mid;
    (void)pt;
    void *p = NULL;
    if (posix_memalign(&p, align, size) != 0)
        return NULL;
    return p;
}

#endif /* TEST_SHIMS_EXTRA */

#ifdef __cplusplus
}
#endif