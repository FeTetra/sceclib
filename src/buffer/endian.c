#include "endian.h"

uint16_t u16_swap(uint16_t n) {
    return 
        (n >> 8) | 
        (n << 8);
}

uint32_t u32_swap(uint32_t n) {
    return 
        ((n & 0x000000FFU) << 24) |
        ((n & 0x0000FF00U) << 8) |
        ((n & 0x00FF0000U) >> 8) |
        ((n & 0xFF000000U) >> 24);
    ;
}

uint64_t u64_swap(uint64_t n) {
    return 
        ((n & 0x00000000000000FFULL) << 56) |
        ((n & 0x000000000000FF00ULL) << 40) |
        ((n & 0x0000000000FF0000ULL) << 24) |
        ((n & 0x00000000FF000000ULL) << 8) |
        ((n & 0x000000FF00000000ULL) >> 8) |
        ((n & 0x0000FF0000000000ULL) >> 24) |
        ((n & 0x00FF000000000000ULL) >> 40) |
        ((n & 0xFF00000000000000ULL) >> 56);
}

uint16_t u16_to_endian(uint16_t n, Endian target) {
    if (target != SYSTEM_ENDIAN) {
        return u16_swap(n);
    }
    return n;
}
uint32_t u32_to_endian(uint32_t n, Endian target) {
    if (target != SYSTEM_ENDIAN) {
        return u32_swap(n);
    }
    return n;
}
uint64_t u64_to_endian(uint64_t n, Endian target) {
    if (target != SYSTEM_ENDIAN) {
        return u64_swap(n);
    }
    return n;
}

uint16_t u16_from_endian(uint16_t n, Endian source) {
    return u16_to_endian(n, source);
}
uint32_t u32_from_endian(uint32_t n, Endian source) {
    return u32_to_endian(n, source);
}
uint64_t u64_from_endian(uint64_t n, Endian source) {
    return u64_to_endian(n, source);
}