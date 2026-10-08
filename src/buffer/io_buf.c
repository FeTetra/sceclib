#include "io_buf.h"

IoBuf *IoBuf_ctor(size_t size, Endian endian, IoBufType type) {
    IoBuf *result = (IoBuf *)malloc(sizeof(IoBuf));
    result->buf = Buf_ctor(size);
    result->pos = 0;
    result->endian = endian;
    result->type = type;
    return result;
}

void IoBuf_dtor(IoBuf *self) {
    Buf_dtor(self->buf);
    memset(self, 0, sizeof(IoBuf));
    free(self);
}

int IoBuf_has_remaining(IoBuf *self, size_t size) {
    return (self->pos + size <= self->buf->size);
}

int IoBuf_expand(IoBuf *self, size_t size) {
    uint8_t *tmp_ptr = realloc(self->buf->data, self->buf->size + size);
    if (!tmp_ptr) {
        return 0; // Could not expand
    }

    self->buf->data = tmp_ptr;
    self->buf->size += size;
    return 1;
}

int IoBuf_ensure_remaining(IoBuf *self, size_t size) {
    size_t required = self->pos + size;
    if (required <= self->buf->size) {
        return 1;
    }

    return IoBuf_expand(self, required - self->buf->size);
}

void IoBuf_seek_to(IoBuf *self, size_t pos) {
    self->pos = pos;
}

void IoBuf_seek_from_cur(IoBuf *self, size_t nbytes) {
    IoBuf_seek_to(self, self->pos + nbytes);
}

int IoBuf_read_size(IoBuf *self, void *out, size_t size) {
    if (IoBuf_has_remaining(self, size)) {
        memcpy(out, self->buf->data + self->pos, size);
        self->pos += size;
        return 1;
    }
    return 0;
}

int IoBuf_read_u8(IoBuf *self, uint8_t *out) {
    return IoBuf_read_size(self, out, sizeof(uint8_t));
}

int IoBuf_read_u16(IoBuf *self, uint16_t *out) {
    int result = IoBuf_read_size(self, out, sizeof(uint16_t));
    *out = u16_from_endian(*out, self->endian);
    return result;
}

int IoBuf_read_u32(IoBuf *self, uint32_t *out) {
    int result = IoBuf_read_size(self, out, sizeof(uint32_t));
    *out = u32_from_endian(*out, self->endian);
    return result;
}

int IoBuf_read_u64(IoBuf *self, uint64_t *out) {
    int result = IoBuf_read_size(self, out, sizeof(uint64_t));
    *out = u64_from_endian(*out, self->endian);
    return result;
}

int IoBuf_read_into_buf(IoBuf *self, Buf *out) {
    return IoBuf_read_size(self, out->data, out->size);
}

int IoBuf_read_size_into_buf(IoBuf *self, Buf *out, size_t size) {
    if (out->size < size ) {
        return 0;
    }
    return IoBuf_read_size(self, out->data, size);
}

int IoBuf_skip(IoBuf *self, size_t count) {
    if (IoBuf_has_remaining(self, count)) {
        self->pos += count;
        return 1;
    }
    return 0;
}

int IoBuf_write_size(IoBuf *self, void *in, size_t size) {
    if(IoBuf_ensure_remaining(self, size)) {
        memcpy(self->buf->data + self->pos, in, size);
        self->pos += size;
        return 1;
    }
    return 0;
}

int IoBuf_write_u8(IoBuf *self, uint8_t in) {
    return IoBuf_write_size(self, &in, sizeof(uint8_t));
}

int IoBuf_write_u16(IoBuf *self, uint16_t in) {
    in = u16_to_endian(in, self->endian);
    return IoBuf_write_size(self, &in, sizeof(uint16_t));
}

int IoBuf_write_u32(IoBuf *self, uint32_t in) {
    in = u32_to_endian(in, self->endian);
    return IoBuf_write_size(self, &in, sizeof(uint32_t));
}

int IoBuf_write_u64(IoBuf *self, uint64_t in) {
    in = u64_to_endian(in, self->endian);
    return IoBuf_write_size(self, &in, sizeof(uint64_t));
}

int IoBuf_write_from_buf(IoBuf *self, Buf *in) {
    return IoBuf_write_size(self, in->data, in->size);
}

int IoBuf_write_size_from_buf(IoBuf *self, Buf *in, size_t size) {
    if (in->size < size) {
        return 0;
    }
    return IoBuf_write_size(self, in->data, size);
}

int IoBuf_pad(IoBuf *self, size_t count) {
    if (IoBuf_ensure_remaining(self, count)) {
        memset((self->buf->data + self->pos), 0, count);
        self->pos += count;
        return 1;
    }
    return 0;
}
