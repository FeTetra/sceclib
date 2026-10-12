#ifndef SCETYPES_PRINT_H
#define SCETYPES_PRINT_H

#include <stdio.h>

#include "scetypes.h"
#include "hexdump.h"

static void SELFHeader_print(SELFHeader *h) {
    printf("Magic: %#x\n", h->magic);
    printf("Version: %#x\n", h->cf_version);
    printf("Attribute: %#x\n", h->attribute);
    printf("Category: %#x\n", h->category);
    printf("Encryption Root Header Offset: %#x\n", h->ext_hdr_size);
    printf("File offset: %#llx\n", h->file_offset);
    printf("File size: %#llx\n", h->file_size);
}

static void EncryptionRootHeader_print(EncryptionRootHeader *erh) {
    printf("ERH Key: ");
    PrintHexLines(erh->key, 0x10, 16, 1);
    printf("ERH Key Pad: ");
    PrintHexLines(erh->key_pad, 0x10, 16, 1);
    printf("ERH IV: ");
    PrintHexLines(erh->iv, 0x10, 16, 1);
    printf("ERH IV Pad: ");
    PrintHexLines(erh->iv_pad, 0x10, 16, 1);
}

static void CertificationHeader_print(CertificationHeader *h) {
    printf("Sign offset: %#llx\n", h->sign_offset);
    printf("Sign algorithm: %d\n", h->sign_algorithm);
    printf("Cert entry number: %d\n", h->cert_entry_num);
    printf("Attr entry number: %d\n", h->attr_entry_num);
    printf("Optional header size: %d\n", h->optional_hdr_size);
}

static void SegmentCertificationHeader_print(SegmentCertificationHeader *h) {
    printf("Segment offset: %#llx\n", h->segment_offset);
    printf("Segment size: %#llx\n", h->segment_size);
    printf("Segment type: %d\n", h->segment_type);
    printf("Segment id: %d\n", h->segment_id);
    printf("Sign algorithm: %d\n", h->sign_algorithm);
    printf("Sign index: %d\n", h->sign_idx);
    printf("Encryption algorithm: %d\n", h->enc_algorithm);
    printf("Key index: %d\n", h->key_idx);
    printf("IV index: %d\n", h->iv_idx);
    printf("Comp algorithm: %d\n", h->comp_algorithm);
}

#endif // SCETYPES_PRINT_H