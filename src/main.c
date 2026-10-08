#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>

#include "buffer/io_buf.h"
#include "buffer/endian.h"

#include "hexdump.h"
#include "test_keys.h" // Bring your own, its a uint8_t array

#define SELF_HDR_SIZE 0x20
#define ENC_ROOT_HDR_SIZE 0x40
#define CERTIFICATION_HDR_SIZE 0x20

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

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

ExtendedHeader ExtendedHeader_read(IoBuf *r) {
    ExtendedHeader result;

    IoBuf_read_u64(r, &result.ext_hdr_version);
    IoBuf_read_u64(r, &result.program_identification_hdr_offset);
    IoBuf_read_u64(r, &result.ehdr_offset);
    IoBuf_read_u64(r, &result.phdr_offset);
    IoBuf_read_u64(r, &result.shdr_offset);
    IoBuf_read_u64(r, &result.segment_ext_hdr_offset);
    IoBuf_read_u64(r, &result.version_hdr_offset);
    IoBuf_read_u64(r, &result.supplemental_hdr_offset);
    IoBuf_read_u64(r, &result.supplemental_hdr_size);
    IoBuf_seek_from_cur(r, 8);

    return result;
}

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

void EncryptionRootHeader_print(EncryptionRootHeader *erh) {
    printf("ERH Key: ");
    PrintHexLines(erh->key, 0x10, 16, 1);
    printf("ERH IV: ");
    PrintHexLines(erh->iv, 0x10, 16, 1);
}

SELFHeader SELFHeader_read(IoBuf *r) {
    SELFHeader result;

    IoBuf_read_u32(r, &result.magic);
    IoBuf_read_u32(r, &result.cf_version);
    IoBuf_read_u16(r, &result.attribute);
    IoBuf_read_u16(r, &result.category);
    IoBuf_read_u32(r, &result.ext_hdr_size);
    IoBuf_read_u64(r, &result.file_offset);
    IoBuf_read_u64(r, &result.file_size);
    if (result.cf_version == 3) {
        IoBuf_read_u64(r, &result.cf_file_size);
        IoBuf_seek_from_cur(r, 8);
    }

    return result;
}

void CertificationHeader_print(CertificationHeader *h) {
    printf("Sign offset: %#llx\n", h->sign_offset);
    printf("Sign algorithm: %d\n", h->sign_algorithm);
    printf("Cert entry number: %d\n", h->cert_entry_num);
    printf("Attr entry number: %d\n", h->attr_entry_num);
    printf("Optional header size: %d\n", h->optional_hdr_size);
}

void SELFHeader_print(SELFHeader *h) {
    printf("Magic: %#x\n", h->magic);
    printf("Version: %#x\n", h->cf_version);
    printf("Attribute: %#x\n", h->attribute);
    printf("Category: %#x\n", h->category);
    printf("Encryption Root Header Offset: %#x\n", h->ext_hdr_size);
    printf("File offset: %#llx\n", h->file_offset);
    printf("File size: %#llx\n", h->file_size);
}

int DecryptRaw(void *in, void *out, size_t size, const void *cipher, void *key, void *iv) {
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        printf("Failed to initialize EVP cipher context\n");
        return 0;
    }

    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        printf("Failed to initialize EVP decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    int len;
    int total = 0;

    if (EVP_DecryptUpdate(ctx, out, &len, in, size) != 1) {
        printf("Failed to decrypt data\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    total += len;

    if (EVP_DecryptFinal(ctx, out + total, &len) != 1) {
        printf("Failed to finalize EVP decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return 0;
    }

    EVP_CIPHER_CTX_free(ctx);
    return 1;
}

EncryptionRootHeader EncryptionRootHeader_Read_Decrypt(IoBuf *r) {
    EncryptionRootHeader result;

    u8 ciphertext[ENC_ROOT_HDR_SIZE];
    u8 plaintext[ENC_ROOT_HDR_SIZE];

    IoBuf_read_size(r, ciphertext, ENC_ROOT_HDR_SIZE);

    DecryptRaw(ciphertext, plaintext, ENC_ROOT_HDR_SIZE, EVP_aes_256_cbc(), (void *)test_key, (void *)test_riv);

    memcpy(result.key, plaintext, 0x10);
    memcpy(result.key_pad, plaintext + 0x10, 0x10);
    memcpy(result.iv, plaintext + 0x20, 0x10);
    memcpy(result.iv_pad, plaintext + 0x30, 0x10);

    return result;
}

CertificationHeader CertificationHeader_read(IoBuf *r, EncryptionRootHeader *h) {
    CertificationHeader result;

    u8 ciphertext[CERTIFICATION_HDR_SIZE];
    u8 plaintext[CERTIFICATION_HDR_SIZE];

    IoBuf_read_size(r, ciphertext, CERTIFICATION_HDR_SIZE);

    DecryptRaw(ciphertext, plaintext, CERTIFICATION_HDR_SIZE, EVP_aes_128_ctr(), h->key, h->iv);

    IoBuf *w = IoBuf_ctor(CERTIFICATION_HDR_SIZE, Big, Writer);
    IoBuf_write_size(w, plaintext, CERTIFICATION_HDR_SIZE);
    IoBuf_seek_to(w, 0);

    IoBuf_read_u64(w, &result.sign_offset);
    IoBuf_read_u32(w, &result.sign_algorithm);
    IoBuf_read_u32(w, &result.cert_entry_num);
    IoBuf_read_u32(w, &result.attr_entry_num);
    IoBuf_read_u32(w, &result.optional_hdr_size);
    IoBuf_seek_from_cur(w, sizeof(u64));

    return result;
}

int main() {
    FILE *fp = fopen("EBOOT.BIN", "rb");
    
    if (fp == NULL) {
        printf("Failed to open file.\n");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0, SEEK_END);
    size_t input_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    IoBuf *file_buf = IoBuf_ctor(input_size, Big, Reader);

    fread(file_buf->buf->data, 1, input_size, fp);

    SELFHeader self_hdr = SELFHeader_read(file_buf);

    SELFHeader_print(&self_hdr);

    size_t enc_root_hdr_offset = SELF_HDR_SIZE + self_hdr.ext_hdr_size;
    size_t certification_hdr_offset = enc_root_hdr_offset + ENC_ROOT_HDR_SIZE;

    IoBuf_seek_to(file_buf, enc_root_hdr_offset);

    EncryptionRootHeader erh = EncryptionRootHeader_Read_Decrypt(file_buf);

    EncryptionRootHeader_print(&erh);

    IoBuf_seek_to(file_buf, certification_hdr_offset);

    CertificationHeader ch = CertificationHeader_read(file_buf, &erh);

    CertificationHeader_print(&ch);
}
