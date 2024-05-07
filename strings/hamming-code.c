#include <stddef.h>
#include <stdint.h>

void encode(const void *data, void *encoded, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        uint8_t byte = ((uint8_t *)data)[i];
        ((uint8_t *)encoded)[i] = byte;
        size_t s1 = ((byte >> 1) & 1u) ^ ((byte >> 3) & 1u) ^
                    ((byte >> 5) & 1u) ^ ((byte >> 7) & 1u);
        size_t s2 = ((byte >> 2) & 1u) ^ ((byte >> 3) & 1u) ^
                    ((byte >> 6) & 1u) ^ ((byte >> 7) & 1u);
        size_t s3 = ((byte >> 4) & 1u) ^ ((byte >> 5) & 1u) ^
                    ((byte >> 6) & 1u) ^ ((byte >> 7) & 1u);
        size_t s4 = ((byte >> 1) & 1u) ^ ((byte >> 3) & 1u) ^
                    ((byte >> 5) & 1u) ^ ((byte >> 7) & 1u) ^
                    ((byte >> 0) & 1u) ^ ((byte >> 2) & 1u) ^
                    ((byte >> 4) & 1u) ^ ((byte >> 6) & 1u);
        ((uint8_t *)encoded)[n + i] = (s1 << 7) + (s2 << 6) + (s3 << 5) +
                                      (s4 << 4) + (s1 << 3) + (s2 << 2) +
                                      (s3 << 1) + (s4 << 0);
    }
}

void decode(const void *encoded, void *data, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        uint8_t xor_syms = ((uint8_t *)encoded)[n + i];
        uint8_t byte = ((uint8_t *)encoded)[i];
        if ((xor_syms >> 4) != (xor_syms & ((1u << 4) - 1))) {
            ((uint8_t *)data)[i] = byte;
        } else {
            size_t s1 = ((byte >> 1) & 1u) ^ ((byte >> 3) & 1u) ^
                        ((byte >> 5) & 1u) ^ ((byte >> 7) & 1u);
            size_t s2 = ((byte >> 2) & 1u) ^ ((byte >> 3) & 1u) ^
                        ((byte >> 6) & 1u) ^ ((byte >> 7) & 1u);
            size_t s3 = ((byte >> 4) & 1u) ^ ((byte >> 5) & 1u) ^
                        ((byte >> 6) & 1u) ^ ((byte >> 7) & 1u);
            size_t s4 = ((byte >> 1) & 1u) ^ ((byte >> 3) & 1u) ^
                        ((byte >> 5) & 1u) ^ ((byte >> 7) & 1u) ^
                        ((byte >> 0) & 1u) ^ ((byte >> 2) & 1u) ^
                        ((byte >> 4) & 1u) ^ ((byte >> 6) & 1u);
            size_t check_s1 = (xor_syms >> 3) & 1u;
            size_t check_s2 = (xor_syms >> 2) & 1u;
            size_t check_s3 = (xor_syms >> 1) & 1u;
            size_t check_s4 = (xor_syms >> 0) & 1u;
            if (s4 == check_s4) {
                ((uint8_t *)data)[i] = byte;
            } else {
                size_t bitflip_pos = (check_s1 ^ s1) + ((check_s2 ^ s2) << 1) +
                                     ((check_s3 ^ s3) << 2);
                byte ^= (1 << bitflip_pos);
                ((uint8_t *)data)[i] = byte;
            }
        }
    }
}