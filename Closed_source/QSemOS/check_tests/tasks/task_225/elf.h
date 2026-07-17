// Minimal stub for elf.h (not available on MinGW)
#ifndef ELF_STUB_H
#define ELF_STUB_H
#include <stdint.h>

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf64_Sxword;

typedef struct { 
    unsigned char e_ident[16]; 
    Elf64_Half e_type; 
    Elf64_Half e_machine; 
    Elf64_Word e_version; 
    Elf64_Addr e_entry; 
    Elf64_Off e_phoff; 
    Elf64_Off e_shoff; 
    Elf64_Word e_flags; 
    Elf64_Half e_ehsize; 
    Elf64_Half e_phentsize; 
    Elf64_Half e_phnum; 
    Elf64_Half e_shentsize; 
    Elf64_Half e_shnum; 
    Elf64_Half e_shstrndx; 
} Elf64_Ehdr;

typedef struct { 
    Elf64_Word p_type; 
    Elf64_Word p_flags; 
    Elf64_Off p_offset; 
    Elf64_Addr p_vaddr; 
    Elf64_Addr p_paddr; 
    Elf64_Xword p_filesz; 
    Elf64_Xword p_memsz; 
    Elf64_Xword p_align; 
} Elf64_Phdr;

typedef struct { 
    Elf64_Word sh_name; 
    Elf64_Word sh_type; 
    Elf64_Xword sh_flags; 
    Elf64_Addr sh_addr; 
    Elf64_Off sh_offset; 
    Elf64_Xword sh_size; 
    Elf64_Word sh_link; 
    Elf64_Word sh_info; 
    Elf64_Xword sh_addralign; 
    Elf64_Xword sh_entsize; 
} Elf64_Shdr;

typedef struct { 
    Elf64_Word st_name; 
    unsigned char st_info; 
    unsigned char st_other; 
    Elf64_Half st_shndx; 
    Elf64_Addr st_value; 
    Elf64_Xword st_size; 
} Elf64_Sym;

typedef struct { 
    Elf64_Addr r_offset; 
    Elf64_Xword r_info; 
    Elf64_Sxword r_addend; 
} Elf64_Rela;

typedef struct { 
    Elf64_Addr r_offset; 
    Elf64_Xword r_info; 
} Elf64_Rel;

typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Sym Elf_Sym;
typedef Elf64_Rela Elf_Rela;
typedef Elf64_Rel Elf_Rel;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Sword Elf_Sword;

// ELF magic constants
#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

// e_ident offsets
#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3

// Program header types
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4

// Section header types
#define SHT_NULL     0
#define SHT_PROGBITS 1
#define SHT_SYMTAB   2
#define SHT_STRTAB   3
#define SHT_RELA     4
#define SHT_REL      9

// Symbol table binding
#define STB_LOCAL  0
#define STB_GLOBAL 1

// Symbol table type
#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2

// ELF_R_SYM and ELF_R_TYPE macros
#define ELF64_R_SYM(info) ((info) >> 32)
#define ELF64_R_TYPE(info) ((info) & 0xFFFFFFFFLL)
#define ELF_R_SYM(info) ELF64_R_SYM(info)
#define ELF_R_TYPE(info) ELF64_R_TYPE(info)

// Additional e_ident offsets
#define EI_CLASS 4
#define EI_DATA  5
#define EI_VERSION 6

// ELF class constants
#define ELFCLASSNONE 0
#define ELFCLASS32   1
#define ELFCLASS64   2

// ELF data encoding
#define ELFDATANONE 0
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// ELF type constants
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4

// Machine architecture constants
#define EM_NONE       0
#define EM_386        3
#define EM_X86_64    62
#define EM_ARM       40
#define EM_AARCH64  183

// ELF version constants
#define EV_NONE    0
#define EV_CURRENT 1

#endif
