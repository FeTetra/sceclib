#ifndef ELF_TYPES_H
#define ELF_TYPES_H

#include "../global_defs.h"

#define EI_NIDENT (16)

typedef u16 Elf32_Half;
typedef u32 Elf32_Word;
typedef	s32 Elf32_Sword;
typedef u64 Elf32_Xword;
typedef	s64 Elf32_Sxword;
typedef u32 Elf32_Addr;
typedef u32 Elf32_Off;
typedef u16 Elf32_Section;
typedef Elf32_Half Elf32_Versym;

typedef u16 Elf64_Half;
typedef u32 Elf64_Word;
typedef	s32 Elf64_Sword;
typedef u64 Elf64_Xword;
typedef	s64 Elf64_Sxword;
typedef u64 Elf64_Addr;
typedef u64 Elf64_Off;
typedef u16 Elf64_Section;
typedef Elf64_Half Elf64_Versym;

#endif // ELF_TYPES_H