#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define OS_MODULE_OK 0
#define OS_MODULE_ERROR 1

#define SHT_RELA 7
#define SHT_REL 17

typedef uint32_t U32;
typedef uint16_t U16;
typedef uint64_t U64;
typedef int32_t S32;
typedef signed char S8;
typedef unsigned char U8;

#define MODULE_UNIT_INIT 1
#define MODULE_UNIT_ACTIVE 2
#define OS_MID_DYNAMIC 0x14

struct TagListObject {
    struct TagListObject *prev;
    struct TagListObject *next;
};

static inline void OS_LIST_INIT(struct TagListObject *head)
{
    head->next = head;
    head->prev = head;
}

struct DynModuleUnitInfo {
    char *moduleStr;
    U32 stDev;
    U32 stIno;
    U32 state;
};

typedef struct {
    U64 st_dev;
    U64 st_ino;
    U64 st_nlink;
} Stat;

struct Elf64_Ehdr {
    U64 e_shoff;
    U16 e_shnum;
};

struct Elf32_Ehdr {
    U32 e_shoff;
    U16 e_shnum;
};

typedef U64 Elf_Addr;
typedef U32 Elf_Off;
typedef U16 Elf_Half;
typedef U32 Elf_Word;

typedef struct Elf64_Shdr {
    Elf_Word sh_name;
    Elf_Word sh_type;
    U64 sh_flags;
    U64 sh_addr;
    U64 sh_offset;
    U64 sh_size;
    Elf_Word sh_link;
    Elf_Word sh_info;
    U64 sh_addralign;
    U64 sh_entsize;
} Elf_Shdr;

typedef struct Elf32_Shdr {
    Elf_Word sh_name;
    Elf_Word sh_type;
    U32 sh_flags;
    U32 sh_addr;
    U32 sh_offset;
    U32 sh_size;
    Elf_Word sh_link;
    Elf_Word sh_info;
    U32 sh_addralign;
    U32 sh_entsize;
} Elf32_Shdr;

typedef struct {
    U32 st_name;
    U32 st_value;
    U32 st_size;
    U8 st_info;
    U8 st_other;
    U16 st_shndx;
} Elf_Sym;

static inline void OsDynModuleSetError(const char *msg, ...) { (void)msg; }

static inline U32 OsDynModuleFileCheck(const char *file, struct stat *statBuf, struct DynModuleUnitInfo **unitInfo) {
    (void)file; (void)statBuf; (void)unitInfo;
    return OS_MODULE_ERROR;
}

static inline U32 OsDynModuleGetFileContent(const char *file, char **content) {
    (void)file; (void)content;
    return OS_MODULE_ERROR;
}

static inline U32 OsDynModuleCheckElf(void *elf) { (void)elf; return OS_MODULE_ERROR; }

static inline U32 OsDynModuleUnitAlloc(struct DynModuleUnitInfo **unitInfo) { 
    (void)unitInfo;
    return OS_MODULE_ERROR;
}

static inline U32 OsDynModuleUnitLoad(struct DynModuleUnitInfo *unitInfo, S32 mode) { 
    (void)unitInfo; (void)mode; 
    return OS_MODULE_ERROR;
}

static inline void OsDynModuleUnitFree(struct DynModuleUnitInfo *unitInfo) { (void)unitInfo; }

static inline void *PRT_MemFree(U32 pool, void *ptr) { (void)pool; free(ptr); return NULL; }

void os_asm_invalidate_dcache_all(void) {}
void os_asm_invalidate_icache_all(void) {}

static inline U32 OsDynModuleRelocatePrepare(struct DynModuleUnitInfo *unitInfo, uintptr_t reloc,
                                              Elf_Sym *symTab, uint8_t *strTab, Elf_Shdr *shdr) {
    (void)unitInfo; (void)reloc; (void)symTab; (void)strTab; (void)shdr;
    return OS_MODULE_OK;
}

#define Elf_Ehdr struct Elf64_Ehdr
