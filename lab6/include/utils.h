#ifndef __UTILS_H
#define __UTILS_H

#include <stddef.h>
#include <stdint.h>

void branchAddr(void *addr);
void memcpy(uint8_t *dst, uint8_t *src, size_t n);

#endif
