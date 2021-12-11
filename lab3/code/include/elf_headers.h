#ifndef CODE_ELF_HEADERS_H
#define CODE_ELF_HEADERS_H

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

/* 32-bit ELF base types. */
typedef __u32 Elf32_Addr;
typedef __u16 Elf32_Half;
typedef __u32 Elf32_Off;
typedef __s32 Elf32_Sword;
typedef __u32 Elf32_Word;

#define SHN_ABS        0xfff1
#define SHN_COMMON     0xfff2
#define SHN_UNDEF      0
#define SHN_XINDEX     0xffff

#define ELF32_ST_VISIBILITY(o)    ((o)&0x3)
#define ELF_ST_BIND(x)        ((x) >> 4)
#define ELF32_ST_TYPE(x)    ELF_ST_TYPE(x)
#define ELF_ST_TYPE(x)        (((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)    ELF_ST_BIND(x)

#define EI_NIDENT    16

// from elf.h
typedef struct elf32_hdr {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;  /* Entry point */
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct elf32_shdr {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct elf32_sym {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

#endif //CODE_ELF_HEADERS_H