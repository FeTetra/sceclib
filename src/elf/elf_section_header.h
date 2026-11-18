#ifndef ELF_SECTION_HEADER_H
#define ELF_SECTION_HEADER_H

#include "elf_types.h"
#include "../buffer/io_buf.h"

typedef struct Elf32_Shdr {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off	sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct Elf64_Shdr {
  Elf64_Word sh_name;
  Elf64_Word sh_type;
  Elf64_Xword sh_flags;
  Elf64_Addr sh_addr;
  Elf64_Off	sh_offset;
  Elf64_Xword sh_size;
  Elf64_Word sh_link;
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;

Elf32_Shdr Elf32_Shdr_read(IoBuf *r);
void Elf32_Shdr_write(IoBuf *w, Elf32_Shdr *h);

Elf64_Shdr Elf64_Shdr_read(IoBuf *r);
void Elf64_Shdr_write(IoBuf *w, Elf64_Shdr *h);

#endif // ELF_SECTION_HEADER_H