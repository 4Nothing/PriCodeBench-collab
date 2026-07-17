#pragma once
#include <stddef.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EOK 0
typedef int errno_t;

#ifdef __cplusplus
extern "C"
{
#endif

    typedef uint32_t U32;
    typedef uint16_t U16;
    typedef uint8_t U8;
    typedef uint64_t U64;
    typedef int32_t S32;

#ifndef OS_MAX_MODULE_NUM
#define OS_MAX_MODULE_NUM 8
#endif

#ifndef OS_MODULE_ALIGN_LEN
#define OS_MODULE_ALIGN_LEN 16
#endif

#ifndef MEM_ADDR_ALIGN_4K
#define MEM_ADDR_ALIGN_4K 4096
#endif

#define ALIGN(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

#ifndef OS_MODULE_OK
#define OS_MODULE_OK 0
#endif
#ifndef OS_MODULE_ERRNO_FILE_OPEN
#define OS_MODULE_ERRNO_FILE_OPEN 1
#endif
#ifndef OS_MODULE_ERRNO_MEMORY_ALLOC
#define OS_MODULE_ERRNO_MEMORY_ALLOC 2
#endif
#ifndef OS_MODULE_ERRNO_FILE_READ
#define OS_MODULE_ERRNO_FILE_READ 3
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_MAGIC
#define OS_MODULE_ERRNO_ELF_HEAD_MAGIC 10
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_CLASS
#define OS_MODULE_ERRNO_ELF_HEAD_CLASS 11
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_LEN
#define OS_MODULE_ERRNO_ELF_HEAD_LEN 12
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE
#define OS_MODULE_ERRNO_ELF_HEAD_SHENT_SIZE 13
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_SHNUM
#define OS_MODULE_ERRNO_ELF_HEAD_SHNUM 14
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_MACHINE
#define OS_MODULE_ERRNO_ELF_HEAD_MACHINE 15
#endif
#ifndef OS_MODULE_ERRNO_ELF_HEAD_TYPE
#define OS_MODULE_ERRNO_ELF_HEAD_TYPE 16
#endif
#ifndef OS_MODULE_ERRNO_PTLOAD_SIZE
#define OS_MODULE_ERRNO_PTLOAD_SIZE 20
#endif
#ifndef OS_MODULE_ERRNO_MEMCOPY
#define OS_MODULE_ERRNO_MEMCOPY 21
#endif
#ifndef OS_MODULE_ERRNO_DYNSYM_SECTION_NOT_FOUND
#define OS_MODULE_ERRNO_DYNSYM_SECTION_NOT_FOUND 30
#endif
#ifndef OS_MODULE_ERRNO_NO_GLOBAL_FUNC
#define OS_MODULE_ERRNO_NO_GLOBAL_FUNC 31
#endif
#ifndef OS_MODULE_ERRNO_UNIT_FULL
#define OS_MODULE_ERRNO_UNIT_FULL 40
#endif
#ifndef OS_MODULE_ERRNO_FILE_CHECK_FAILED
#define OS_MODULE_ERRNO_FILE_CHECK_FAILED 50
#endif

    enum ModuleUnitState
    {
        MODULE_UNIT_FREE = 0,
        MODULE_UNIT_INIT = 1,
        MODULE_UNIT_ACTIVE = 2,
    };

#include <elf.h>
    typedef Elf64_Ehdr Elf_Ehdr;
    typedef Elf64_Phdr Elf_Phdr;
    typedef Elf64_Shdr Elf_Shdr;
    typedef Elf64_Sym Elf_Sym;
    typedef Elf64_Rela Elf_Rela;
    typedef Elf64_Rel Elf_Rel;
    typedef Elf64_Addr Elf_Addr;
    typedef Elf64_Sword Elf_Sword;
    typedef Elf64_Word Elf_Word;

#ifndef ELF_R_SYM
#define ELF_R_SYM ELF64_R_SYM
#endif
#ifndef ELF_R_TYPE
#define ELF_R_TYPE ELF64_R_TYPE
#endif
#ifndef ELF_ST_BIND
#define ELF_ST_BIND ELF64_ST_BIND
#endif
#ifndef ELF_ST_TYPE
#define ELF_ST_TYPE ELF64_ST_TYPE
#endif
#ifndef ELF_ST_INFO
#define ELF_ST_INFO ELF64_ST_INFO
#endif
#ifndef ELF_R_INFO
#define ELF_R_INFO ELF64_R_INFO
#endif

    typedef struct DynModuleSymTab
    {
        char *name;
        void *addr;
    } DynModuleSymTab;

    struct DynModuleUnitInfo
    {
        U8 unitNo;
        U16 symNum;
        enum ModuleUnitState state;
        U64 loadSegStartAddr;
        U64 loadSegEndAddr;
        dev_t stDev;
        ino_t stIno;
        struct DynModuleSymTab *symTab;
        uint8_t *loadSegMem;
        char *moduleStr;
        char *error;
    };

#define OS_MID_DYNAMIC 0
#define OS_MEM_DEFAULT_FSC_PT 0

    static inline void *PRT_MemAlloc(int mid, int pt, size_t size)
    {
        (void)mid;
        (void)pt;
        return malloc(size);
    }
    static inline void *PRT_MemAllocAlign(int mid, int pt, size_t size, size_t align)
    {
        (void)mid;
        (void)pt;
        (void)align;
        void *p = NULL;
        if (posix_memalign(&p, align, size) != 0)
            return NULL;
        return p;
    }
    static inline void PRT_MemFree(int mid, void *ptr)
    {
        (void)mid;
        free(ptr);
    }

    static inline uintptr_t OsModuleIntLock(void) { return 0; }
    static inline void OsModuleIntUnlock(uintptr_t s) { (void)s; }

    static inline uintptr_t OsDynModuleFindSymFromOs(const char *name)
    {
        (void)name;
        return 0;
    }

    struct OsDynModuleRelocInfo
    {
        uintptr_t reloc;
        unsigned int relocType;
        unsigned int shType;
        uintptr_t symAddr;
    };
#ifndef TEST_EXTERNAL_RELOCATE
    static inline U32 OsDynModuleRelocate(uintptr_t relocAddr, struct OsDynModuleRelocInfo info)
    {
        (void)relocAddr;
        (void)info;
        return OS_MODULE_OK;
    }
#endif

    static inline int vsnprintf_s(char *dest, size_t destsz, size_t maxcount, const char *format, va_list ap)
    {
        (void)destsz;
        return vsnprintf(dest, maxcount, format, ap);
    }
    static inline int strcpy_s(char *dest, size_t destsz, const char *src)
    {
        strncpy(dest, src, destsz);
        if (destsz > 0) {
            dest[destsz - 1] = '\0';
        }
        return 0;
    }
    static inline int memcpy_s(void *dest, size_t destsz, const void *src, size_t count)
    {
        (void)destsz;
        memcpy(dest, src, count);
        return 0;
    }

    static inline void os_asm_invalidate_dcache_all(void) { }
    static inline void os_asm_invalidate_icache_all(void) { }

#ifndef OS_MODULE_ERROR_STR_LEN
#define OS_MODULE_ERROR_STR_LEN 256
#endif

#ifdef __cplusplus
}
#endif
