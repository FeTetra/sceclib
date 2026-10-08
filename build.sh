#!/bin/bash

CSRCS="src/buffer/buffer.c src/buffer/io_buf.c src/buffer/endian.c"
CSRCS="$CSRCS src/main.c"
CFLAGS="-g"
INCLUDES="-lcrypto"

# For macos
LDFLAGS="-L/opt/homebrew/opt/openssl@3/lib"
INCLUDES="$INCLUDES -I/opt/homebrew/opt/openssl@3/include"

gcc $CFLAGS $CSRCS $INCLUDES $LDFLAGS