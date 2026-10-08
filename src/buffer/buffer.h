#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct Buf {
    uint8_t *data;
    size_t size;
} Buf;

Buf *Buf_ctor(size_t size);
void Buf_dtor(Buf *self);

#endif // BUFFER_H