#include "buffer.h"

Buf *Buf_ctor(size_t size) {
    Buf *result = malloc(sizeof(Buf));
    result->data = malloc(size);
    result->size = size;
    return result;
}

void Buf_dtor(Buf *self) {
    free(self->data);
    memset(self, 0, sizeof(Buf));
    free(self);
}