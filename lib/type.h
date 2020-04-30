#ifndef __TYPE_H
#define __TYPE_H

#ifndef NULL
    #define NULL ((void *)0)
#endif

enum type_t {
    INT = 0,
    DOUBLE = 1,
    CHAR = 2,
    STRING = 3,
    LONG_INT = 4,
    
    BINARY = 5,
    HEXADECIMAL = 6,
    OCTAL = 7,

    POINTER = 8,
};

typedef int int32_t;
typedef long long int int64_t;

typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

#endif