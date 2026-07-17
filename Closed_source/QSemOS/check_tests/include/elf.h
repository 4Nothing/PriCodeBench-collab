/* Minimal stub for <elf.h> — provides ELF64 types used by qsem_test_types.h.
 * These are only used as opaque type names in the test infrastructure. */
#ifndef STUB_ELF_H
#define STUB_ELF_H

typedef struct { unsigned char dummy[64]; } Elf64_Ehdr;
typedef struct { unsigned char dummy[56]; } Elf64_Phdr;
typedef struct { unsigned char dummy[64]; } Elf64_Shdr;
typedef struct { unsigned char dummy[24]; } Elf64_Sym;
typedef unsigned long long Elf64_Addr;
typedef int Elf64_Sword;
typedef unsigned int Elf64_Word;
typedef struct { unsigned char dummy[24]; } Elf64_Rela;
typedef struct { unsigned char dummy[16]; } Elf64_Rel;

#endif /* STUB_ELF_H */
