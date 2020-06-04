#ifndef MATH_H
#define MATH_H

#include "type.h"

int pow ( int base, int exponent );
uint64_t round_to_next_power_2 ( uint64_t n );
uint64_t log2 ( uint64_t n );
int find_first_0_in_bit ( uint64_t n );

#endif