#include "elf_section_header.h"

Elf32_Shdr Elf32_Shdr_read(IoBuf *r) {
    Elf32_Shdr result;
    IoBuf_read_u32(r, &result.sh_name);
    IoBuf_read_u32(r, &result.sh_type);
    IoBuf_read_u32(r, &result.sh_flags);
    IoBuf_read_u32(r, &result.sh_addr);
    IoBuf_read_u32(r, &result.sh_offset);
    IoBuf_read_u32(r, &result.sh_size);
    IoBuf_read_u32(r, &result.sh_link);
    IoBuf_read_u32(r, &result.sh_info);
    IoBuf_read_u32(r, &result.sh_addralign);
    IoBuf_read_u32(r, &result.sh_entsize);
    return result;
}

void Elf32_Shdr_write(IoBuf *w, Elf32_Shdr *h) {
    IoBuf_write_u32(w, h->sh_name);
    IoBuf_write_u32(w, h->sh_type);
    IoBuf_write_u32(w, h->sh_flags);
    IoBuf_write_u32(w, h->sh_addr);
    IoBuf_write_u32(w, h->sh_offset);
    IoBuf_write_u32(w, h->sh_size);
    IoBuf_write_u32(w, h->sh_link);
    IoBuf_write_u32(w, h->sh_info);
    IoBuf_write_u32(w, h->sh_addralign);
    IoBuf_write_u32(w, h->sh_entsize);
}

Elf64_Shdr Elf64_Shdr_read(IoBuf *r) {
    Elf64_Shdr result;
    IoBuf_read_u32(r, &result.sh_name);
    IoBuf_read_u32(r, &result.sh_type);
    IoBuf_read_u64(r, &result.sh_flags);
    IoBuf_read_u64(r, &result.sh_addr);
    IoBuf_read_u64(r, &result.sh_offset);
    IoBuf_read_u64(r, &result.sh_size);
    IoBuf_read_u32(r, &result.sh_link);
    IoBuf_read_u32(r, &result.sh_info);
    IoBuf_read_u64(r, &result.sh_addralign);
    IoBuf_read_u64(r, &result.sh_entsize);
    return result;
}

void Elf64_Shdr_write(IoBuf *w, Elf64_Shdr *h) {
    IoBuf_write_u32(w, h->sh_name);
    IoBuf_write_u32(w, h->sh_type);
    IoBuf_write_u64(w, h->sh_flags);
    IoBuf_write_u64(w, h->sh_addr);
    IoBuf_write_u64(w, h->sh_offset);
    IoBuf_write_u64(w, h->sh_size);
    IoBuf_write_u32(w, h->sh_link);
    IoBuf_write_u32(w, h->sh_info);
    IoBuf_write_u64(w, h->sh_addralign);
    IoBuf_write_u64(w, h->sh_entsize);
}
