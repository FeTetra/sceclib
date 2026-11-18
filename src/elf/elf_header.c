#include "elf_header.h"

Elf32_Ehdr Elf32_Ehdr_read(IoBuf *r) {
    Elf32_Ehdr result;
    IoBuf_read_size(r, &result.e_ident, EI_NIDENT);
    IoBuf_read_u16(r, &result.e_type);
    IoBuf_read_u16(r, &result.e_machine);
    IoBuf_read_u32(r, &result.e_version);
    IoBuf_read_u32(r, &result.e_entry);
    IoBuf_read_u32(r, &result.e_phoff);
    IoBuf_read_u32(r, &result.e_shoff);
    IoBuf_read_u32(r, &result.e_flags);
    IoBuf_read_u16(r, &result.e_ehsize);
    IoBuf_read_u16(r, &result.e_phentsize);
    IoBuf_read_u16(r, &result.e_phnum);
    IoBuf_read_u16(r, &result.e_shentsize);
    IoBuf_read_u16(r, &result.e_shnum);
    IoBuf_read_u16(r, &result.e_shstrndx);
    return result;
}

void Elf32_Ehdr_write(IoBuf *w, Elf32_Ehdr *h) {
    IoBuf_write_size(w, h->e_ident, EI_NIDENT);
    IoBuf_write_u16(w, h->e_type);
    IoBuf_write_u16(w, h->e_machine);
    IoBuf_write_u32(w, h->e_version);
    IoBuf_write_u32(w, h->e_entry);
    IoBuf_write_u32(w, h->e_phoff);
    IoBuf_write_u32(w, h->e_shoff);
    IoBuf_write_u32(w, h->e_flags);
    IoBuf_write_u16(w, h->e_ehsize);
    IoBuf_write_u16(w, h->e_phentsize);
    IoBuf_write_u16(w, h->e_phnum);
    IoBuf_write_u16(w, h->e_shentsize);
    IoBuf_write_u16(w, h->e_shnum);
    IoBuf_write_u16(w, h->e_shstrndx);
}

Elf64_Ehdr Elf64_Ehdr_read(IoBuf *r) {
    Elf64_Ehdr result;
    IoBuf_read_size(r, &result.e_ident, EI_NIDENT);
    IoBuf_read_u16(r, &result.e_type);
    IoBuf_read_u16(r, &result.e_machine);
    IoBuf_read_u32(r, &result.e_version);
    IoBuf_read_u64(r, &result.e_entry);
    IoBuf_read_u64(r, &result.e_phoff);
    IoBuf_read_u64(r, &result.e_shoff);
    IoBuf_read_u32(r, &result.e_flags);
    IoBuf_read_u16(r, &result.e_ehsize);
    IoBuf_read_u16(r, &result.e_phentsize);
    IoBuf_read_u16(r, &result.e_phnum);
    IoBuf_read_u16(r, &result.e_shentsize);
    IoBuf_read_u16(r, &result.e_shnum);
    IoBuf_read_u16(r, &result.e_shstrndx);
    return result;
}

void Elf64_Ehdr_write(IoBuf *w, Elf64_Ehdr *h) {
    IoBuf_write_size(w, h->e_ident, EI_NIDENT);
    IoBuf_write_u16(w, h->e_type);
    IoBuf_write_u16(w, h->e_machine);
    IoBuf_write_u32(w, h->e_version);
    IoBuf_write_u64(w, h->e_entry);
    IoBuf_write_u64(w, h->e_phoff);
    IoBuf_write_u64(w, h->e_shoff);
    IoBuf_write_u32(w, h->e_flags);
    IoBuf_write_u16(w, h->e_ehsize);
    IoBuf_write_u16(w, h->e_phentsize);
    IoBuf_write_u16(w, h->e_phnum);
    IoBuf_write_u16(w, h->e_shentsize);
    IoBuf_write_u16(w, h->e_shnum);
    IoBuf_write_u16(w, h->e_shstrndx);
}

