#ifndef SCETYPES_H
#define SCETYPES_H

#include <stddef.h>

#include "global_defs.h"

#define SELF_HDR_SIZE 0x20
#define ENC_ROOT_HDR_SIZE 0x40
#define CERTIFICATION_HDR_SIZE 0x20
#define SEGMENT_CERTIFICATION_HDR_SIZE 0x30
#define CERTIFICATION_ATTRIBUTE_HDR_SIZE 0x80

typedef struct SELFHeader {
    u32 magic;
    u32 cf_version;
    u16 attribute;
    u16 category;
    u32 ext_hdr_size;
    u64 file_offset;
    u64 file_size;
    u64 cf_file_size; // For cf version 3
    // Pad 8 bytes after
} SELFHeader;

typedef struct ExtendedHeader {
  u64 ext_hdr_version;
  u64 program_identification_hdr_offset;
  u64 ehdr_offset;
  u64 phdr_offset;
  u64 shdr_offset;
  u64 segment_ext_hdr_offset;
  u64 version_hdr_offset;
  u64 supplemental_hdr_offset;
  u64 supplemental_hdr_size;
  // Pad 8 bytes after
} ExtendedHeader;

typedef struct ProgramIdentificationHeader {
    u64 program_authority_id;
    u32 program_vendor_id;
    u32 program_type;
    u64 program_sceversion;
}ProgramIdentificationHeader;

typedef struct EncryptionRootHeader {
    u8 key[0x10];
    u8 key_pad[0x10];
    u8 iv[0x10];
    u8 iv_pad[0x10];
} EncryptionRootHeader;

typedef struct CertificationHeader {
    u64 sign_offset;
    u32 sign_algorithm;
    u32 cert_entry_num;
    u32 attr_entry_num;
    u32 optional_hdr_size;
    // Pad 8 bytes after
} CertificationHeader;

typedef struct SegmentCertificationHeader {
    u64 segment_offset;
	u64 segment_size;
	u32 segment_type;
	u32 segment_id;
	u32 sign_algorithm;
	u32 sign_idx;
	u32 enc_algorithm;
	u32 key_idx;
	u32 iv_idx;
	u32 comp_algorithm;
} SegmentCertificationHeader;

typedef struct CertificationAttributeHeader {
    u8 signature[0x20];
    u8 sign_key[0x40];
    u8 key[0x10];
    u8 iv[0x10];
} CertificationAttributeHeader;

#endif // SCETYPES_H