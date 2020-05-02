#ifndef __LIB_MATH_H
#define __LIB_MATH_H

#include "type.h"

int pow ( int base, int exponent );
int abs ( int n );
int find_first_0_in_bit ( uint64_t n );
uint64_t mask_0_in_bit ( uint64_t n, uint64_t d );

#endif