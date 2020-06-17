#include "utils.h"

void memcpy(uint8_t *dst, uint8_t *src, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        dst[i] = src[i];
    }
}
