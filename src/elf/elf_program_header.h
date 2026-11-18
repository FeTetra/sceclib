#ifndef ELF_PROGRAM_HEADER_H
#define ELF_PROGRAM_HEADER_H

#include "elf_types.h"
#include "../buffer/io_buf.h"

typedef struct Elf32_Phdr {
  Elf32_Word p_type;
  Elf32_Off	p_offset;
  Elf32_Addr p_vaddr;
  Elf32_Addr p_paddr;
  Elf32_Word p_filesz;
  Elf32_Word p_memsz;
  Elf32_Word p_flags;
  Elf32_Word p_align;
} Elf32_Phdr;

typedef struct Elf64_Phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off	p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

Elf32_Phdr Elf32_Phdr_read(IoBuf *r);
void Elf32_Phdr_write(IoBuf *r, Elf32_Phdr *h);

Elf64_Phdr Elf64_Phdr_read(IoBuf *r);
void Elf64_Phdr_write(IoBuf *r, Elf64_Phdr *h);

#endif // ELF_PROGRAM_HEADER_H