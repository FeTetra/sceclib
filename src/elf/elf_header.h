#ifndef ELF_HEADER_H
#define ELF_HEADER_H

#include "elf_types.h"
#include "../buffer/io_buf.h"

typedef struct Elf32_Ehdr {
  unsigned char	e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off	e_phoff;
  Elf32_Off	e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct Elf64_Ehdr {
  unsigned char	e_ident[EI_NIDENT];
  Elf64_Half e_type;	
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off	e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

Elf32_Ehdr Elf32_Ehdr_read(IoBuf *r);
void Elf32_Ehdr_write(IoBuf *w, Elf32_Ehdr *h);

Elf64_Ehdr Elf64_Ehdr_read(IoBuf *r);
void Elf64_Ehdr_write(IoBuf *w, Elf64_Ehdr *h);

#endif // ELF_HEADER_H