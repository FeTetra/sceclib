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
    void *cipher_ctx;
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

CertificationAttributeHeader CertificationAttributeHeader_read(IoBuf *r, SegmentCertificationHeader *h) {
    CertificationAttributeHeader result;

    if (h->sign_algorithm == 2) {
        IoBuf_read_size(r, result.signature, 0x20);
        IoBuf_read_size(r, result.sign_key, 0x40);
    }
    if (h->sign_algorithm == 3) {
        IoBuf_read_size(r, result.signature, 0x20);
    }
    if (h->sign_algorithm == 6) {
        IoBuf_read_size(r, result.signature, 0x20);
        IoBuf_read_size(r, result.sign_key, 0x20);
    }
    
    if (h->enc_algorithm == 3) {
        IoBuf_read_size(r, result.key, 0x10);
        IoBuf_read_size(r, result.iv, 0x10);
    }

    return result;
}

int WriteIoBufToFile(IoBuf *r, char *file_name) {
    FILE *fp = fopen(file_name, "wb");
    if (fp == NULL) {
        printf("Failed to open file\n");
        return 0;
    }

    size_t written = fwrite(r->buf->data, 1, r->buf->size, fp);
    if (written != r->buf->size) {
        printf("Size written did not match buffer size\n");
        return 0;
    }

    if (fclose(fp) != 0) {
        printf("Failed to close file\n");
        return 0;
    }

    return 1;
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
    fclose(fp);

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

    size_t cert_size = self_hdr.file_offset - (SELF_HDR_SIZE + self_hdr.ext_hdr_size + ENC_ROOT_HDR_SIZE);
    IoBuf *cert_plaintext = IoBuf_ctor(cert_size, Big, Writer);
    DecryptToIoBuf(file_buf, cert_plaintext, cert_size, cert_enc_ctx);

    WriteIoBufToFile(cert_plaintext, "cert_plaintext");

    CertificationHeader ch = CertificationHeader_read(cert_plaintext);
    CertificationHeader_print(&ch);

    printf("\n_________\n\n Segment Certification Headers \n_________\n\n");
    
    SegmentCertificationHeader *schs = malloc(SEGMENT_CERTIFICATION_HDR_SIZE * ch.cert_entry_num);
    for (int i = 0; i < ch.cert_entry_num; i++) {
        printf("Header %d:\n", i);
        schs[i] = SegmentCertificationHeader_read(cert_plaintext);
        SegmentCertificationHeader_print(&schs[i]);
        printf("\n");
    }

    printf("\n_________\n\n Certification Attributes \n_________\n\n");

    CertificationAttributeHeader *cahs = malloc(CERTIFICATION_ATTRIBUTE_HDR_SIZE * ch.cert_entry_num);
    for (int i = 0; i < ch.cert_entry_num; i++) {
        printf("Attribute %d:\n", i);
        cahs[i] = CertificationAttributeHeader_read(cert_plaintext, &schs[i]);
        CertificationAttributeHeader_print(&cahs[i]);
        printf("\n");
    }

    IoBuf_dtor(cert_plaintext);
    
    printf("\n_________\n\n Certification Segments \n_________\n\n");

    // TODO: Decrypt and/or decompress all certification segments

    for (int i = 0; i < ch.cert_entry_num; i++) {
        IoBuf *segment_plaintext = IoBuf_ctor(schs[i].segment_size, Big, Writer);

        IoBuf_seek_to(file_buf, schs[i].segment_offset);

        char file_name[0x10];
        snprintf(file_name, 0x10, "schs[%d]", i);

        if (schs[i].enc_algorithm == 3) {
            EncryptionContext *segment_enc_ctx = NewEncryptionContext(
                EVP_aes_128_ctr(), cahs[schs[i].key_idx].key, cahs[schs[i].iv_idx].iv);
            DecryptToIoBuf(file_buf, segment_plaintext, schs[i].segment_size, cert_enc_ctx);
            FinalizeEncryptionContext(segment_plaintext, segment_enc_ctx);
        } else if (schs[i].enc_algorithm == 1) {
            u8 *fuck = malloc(schs[i].segment_size);
            IoBuf_read_size(file_buf, fuck, schs[i].segment_size);
            IoBuf_write_size(segment_plaintext, fuck, schs[i].segment_size);
            free(fuck);
        }

        WriteIoBufToFile(segment_plaintext, file_name);

        IoBuf_dtor(segment_plaintext); // ew
    }

    free(schs);
    free(cahs);
}
