#ifndef IO_BUF_H
#define IO_BUF_H

#include <stddef.h>
#include <stdint.h>

#include "buffer.h"
#include "endian.h"

typedef enum IoBufType {
    Reader = 0,
    Writer = 1,
} IoBufType;

typedef struct IoBuf {
    Buf *buf;
    size_t pos;
    Endian endian;
    IoBufType type;
} IoBuf;

IoBuf *IoBuf_ctor(size_t size, Endian endian, IoBufType type);
void IoBuf_dtor(IoBuf *self);

int IoBuf_has_remaining(IoBuf *self, size_t size);
int IoBuf_expand(IoBuf *self, size_t size);
int IoBuf_ensure_remaining(IoBuf *self, size_t size);

void IoBuf_seek_to(IoBuf *self, size_t pos);
void IoBuf_seek_from_cur(IoBuf *self, size_t nbytes);

int IoBuf_skip(IoBuf *self, size_t count);
int IoBuf_pad(IoBuf *self, size_t count);

int IoBuf_read_size(IoBuf *self, void *out, size_t size);
int IoBuf_read_u8(IoBuf *self, uint8_t *out);
int IoBuf_read_u16(IoBuf *self, uint16_t *out);
int IoBuf_read_u32(IoBuf *self, uint32_t *out);
int IoBuf_read_u64(IoBuf *self, uint64_t *out);
int IoBuf_read_into_buf(IoBuf *self, Buf *out);
int IoBuf_read_size_into_buf(IoBuf *self, Buf *out, size_t size);

int IoBuf_write_size(IoBuf *self, void *in, size_t size);
int IoBuf_write_u8(IoBuf *self, uint8_t in);
int IoBuf_write_u16(IoBuf *self, uint16_t in);
int IoBuf_write_u32(IoBuf *self, uint32_t in);
int IoBuf_write_u64(IoBuf *self, uint64_t in);
int IoBuf_write_from_buf(IoBuf *self, Buf *in);
int IoBuf_write_size_from_buf(IoBuf *self, Buf *in, size_t size);

int IoBuf_copy_size(IoBuf *self, IoBuf *in, size_t size);
int IoBuf_copy_entire(IoBuf *self, IoBuf *in);

#endif // IO_BUF_H