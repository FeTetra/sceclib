#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <stdio.h>

#include "buffer/io_buf.h"

static void PrintHexLines(const unsigned char *src, size_t size, size_t per_line, size_t lines) {
    size_t max = per_line * lines;

    for (size_t i = 0; i < max && i < size; i++) {
        if (i > 0 && i % per_line == 0)
            printf("\n");

        if (i % per_line == per_line / 2)
            printf(" ");

        printf("%02x", src[i]);
    }

    printf("\n");
}


#endif // HEXDUMP_H