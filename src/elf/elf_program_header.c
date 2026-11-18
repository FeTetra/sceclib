#include "elf_program_header.h"

Elf32_Phdr Elf32_Phdr_read(IoBuf *r) {
    Elf32_Phdr result;
    IoBuf_read_u32(r, &result.p_type);
    IoBuf_read_u32(r, &result.p_flags);
    IoBuf_read_u32(r, &result.p_offset);
    IoBuf_read_u32(r, &result.p_vaddr);
    IoBuf_read_u32(r, &result.p_paddr);
    IoBuf_read_u32(r, &result.p_filesz);
    IoBuf_read_u32(r, &result.p_memsz);
    IoBuf_read_u32(r, &result.p_align);
    return result;
}

void Elf32_Phdr_write(IoBuf *r, Elf32_Phdr *h) {
    IoBuf_write_u32(r, h->p_type);
    IoBuf_write_u32(r, h->p_flags);
    IoBuf_write_u32(r, h->p_offset);
    IoBuf_write_u32(r, h->p_vaddr);
    IoBuf_write_u32(r, h->p_paddr);
    IoBuf_write_u32(r, h->p_filesz);
    IoBuf_write_u32(r, h->p_memsz);
    IoBuf_write_u32(r, h->p_align);
}

Elf64_Phdr Elf64_Phdr_read(IoBuf *r) {
    Elf64_Phdr result;
    IoBuf_read_u32(r, &result.p_type);
    IoBuf_read_u32(r, &result.p_flags);
    IoBuf_read_u64(r, &result.p_offset);
    IoBuf_read_u64(r, &result.p_vaddr);
    IoBuf_read_u64(r, &result.p_paddr);
    IoBuf_read_u64(r, &result.p_filesz);
    IoBuf_read_u64(r, &result.p_memsz);
    IoBuf_read_u64(r, &result.p_align);
    return result;
}

void Elf64_Phdr_write(IoBuf *r, Elf64_Phdr *h) {
    IoBuf_write_u32(r, h->p_type);
    IoBuf_write_u32(r, h->p_flags);
    IoBuf_write_u64(r, h->p_offset);
    IoBuf_write_u64(r, h->p_vaddr);
    IoBuf_write_u64(r, h->p_paddr);
    IoBuf_write_u64(r, h->p_filesz);
    IoBuf_write_u64(r, h->p_memsz);
    IoBuf_write_u64(r, h->p_align);
}
