#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/evp.h>
#include <openssl/err.h>

#include "buffer/io_buf.h"
#include "buffer/endian.h"

#include "global_defs.h"
#include "scetypes.h"

#include "hexdump.h"
#include "scetypes_print.h"
#include "test_keys.h" // Bring your own, its a uint8_t array

typedef struct EncryptionContext {
    EVP_CIPHER_CTX *cipher_ctx;
    int total;
} EncryptionContext;

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

EncryptionContext *NewEncryptionContext(const void *cipher, void *key, void *iv) {
    EncryptionContext *result = malloc(sizeof(EncryptionContext));

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

    if (!ctx) {
        printf("Failed to initialize EVP cipher context\n");
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, cipher, NULL, key, iv) != 1) {
        printf("Failed to initialize EVP decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx);
        return NULL;
    }
    EVP_CIPHER_CTX_set_padding(ctx, 0);

    result->cipher_ctx = ctx;
    result->total = 0;

    return result;
}

int FinalizeEncryptionContext(IoBuf *r, EncryptionContext *ctx) {
    int len = 0;
    if (EVP_DecryptFinal(ctx->cipher_ctx, r->buf->data + ctx->total, &len) != 1) { // bad idea
        printf("Failed to finalize EVP decryption\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx->cipher_ctx);
        return 0;
    }

    EVP_CIPHER_CTX_free(ctx->cipher_ctx);
    memset(ctx, 0, sizeof(EncryptionContext));
    free(ctx);

    return 1;
}

int DecryptToIoBuf(IoBuf *r, IoBuf *w, size_t size, EncryptionContext *ctx) {
    u8 *ciphertext = malloc(size);
    u8 *plaintext = malloc(size);
    IoBuf_read_size(r, ciphertext, size);

    int len = 0;
    if (EVP_DecryptUpdate(ctx->cipher_ctx, plaintext, &len, ciphertext, size) != 1) {
        printf("Failed to decrypt data\n");
        ERR_print_errors_fp(stderr);
        EVP_CIPHER_CTX_free(ctx->cipher_ctx);
        return 0;
    }

    ctx->total += len;

    // I could techinically write to w->buf->data directly and skip this copy, but safety or whatever
    IoBuf_write_size(w, plaintext, size);
    IoBuf_seek_from_cur(w, -size);
    free(plaintext);
    free(ciphertext);

    return 1;
}

EncryptionRootHeader EncryptionRootHeader_read(IoBuf *r) {
    EncryptionRootHeader result;

    IoBuf_read_size(r, result.key, 0x10);
    IoBuf_read_size(r, result.key_pad, 0x10);
    IoBuf_read_size(r, result.iv, 0x10);
    IoBuf_read_size(r, result.iv_pad, 0x10);

    return result;
}

CertificationHeader CertificationHeader_read(IoBuf *r) {
    CertificationHeader result;

    IoBuf_read_u64(r, &result.sign_offset);
    IoBuf_read_u32(r, &result.sign_algorithm);
    IoBuf_read_u32(r, &result.cert_entry_num);
    IoBuf_read_u32(r, &result.attr_entry_num);
    IoBuf_read_u32(r, &result.optional_hdr_size);
    IoBuf_seek_from_cur(r, sizeof(u64));

    return result;
}



SegmentCertificationHeader SegmentCertificationHeader_read(IoBuf *r) {
    SegmentCertificationHeader result;

    IoBuf_read_u64(r, &result.segment_offset);
    IoBuf_read_u64(r, &result.segment_size);
    IoBuf_read_u32(r, &result.segment_type);
    IoBuf_read_u32(r, &result.segment_id);
    IoBuf_read_u32(r, &result.sign_algorithm);
    IoBuf_read_u32(r, &result.sign_idx);
    IoBuf_read_u32(r, &result.enc_algorithm);
    IoBuf_read_u32(r, &result.key_idx);
    IoBuf_read_u32(r, &result.iv_idx);
    IoBuf_read_u32(r, &result.comp_algorithm);

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

    printf("\n_________\n\n Certified File Header \n_________\n\n");

    SELFHeader self_hdr = SELFHeader_read(file_buf);
    SELFHeader_print(&self_hdr);

    size_t enc_root_hdr_offset = SELF_HDR_SIZE + self_hdr.ext_hdr_size;
    size_t certification_hdr_offset = enc_root_hdr_offset + ENC_ROOT_HDR_SIZE;
    size_t segment_certification_hdr_offset = certification_hdr_offset + CERTIFICATION_HDR_SIZE;

    IoBuf_seek_to(file_buf, enc_root_hdr_offset);

    printf("\n_________\n\n Encryption Root Header \n_________\n\n");

    EncryptionContext *erh_enc_ctx = NewEncryptionContext(EVP_aes_256_cbc(), (void *)test_key, (void *)test_riv);
    IoBuf *erh_plaintext = IoBuf_ctor(ENC_ROOT_HDR_SIZE, Big, Writer);
    DecryptToIoBuf(file_buf, erh_plaintext, ENC_ROOT_HDR_SIZE, erh_enc_ctx);
    FinalizeEncryptionContext(erh_plaintext, erh_enc_ctx);
    EncryptionRootHeader erh = EncryptionRootHeader_read(erh_plaintext);
    IoBuf_dtor(erh_plaintext);
    EncryptionRootHeader_print(&erh);

    printf("\n_________\n\n Certification Headers \n_________\n\n");

    IoBuf_seek_to(file_buf, certification_hdr_offset);

    EncryptionContext *cert_enc_ctx = NewEncryptionContext( EVP_aes_128_ctr(), erh.key, erh.iv);
    IoBuf *cert_plaintext = IoBuf_ctor(CERTIFICATION_HDR_SIZE, Big, Writer);

    DecryptToIoBuf(file_buf, cert_plaintext, CERTIFICATION_HDR_SIZE, cert_enc_ctx);
    CertificationHeader ch = CertificationHeader_read(cert_plaintext);
    IoBuf_dtor(cert_plaintext);
    CertificationHeader_print(&ch);

    printf("\n_________\n\n Segment Certification Headers \n_________\n\n");

    // Might wanna do this differently to spend less time on reallocation
    cert_plaintext = IoBuf_ctor(SEGMENT_CERTIFICATION_HDR_SIZE * ch.cert_entry_num, Big, Writer);
    DecryptToIoBuf(file_buf, cert_plaintext, SEGMENT_CERTIFICATION_HDR_SIZE * ch.cert_entry_num, cert_enc_ctx);
    for (int i = 0; i < ch.cert_entry_num; i++) {
        printf("Header %d:\n", i);
        SegmentCertificationHeader sch = SegmentCertificationHeader_read(cert_plaintext);
        SegmentCertificationHeader_print(&sch);
        printf("\n");
    }
}
