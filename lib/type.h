#ifndef __LIB_TYPE_H
#define __LIB_TYPE_H

#ifndef NULL
#define NULL ( (void *) 0 )
#endif

enum type_t
{
    INT = 0,
    DOUBLE,
    CHAR,
    STRING,
    LONG_INT,

    BINARY,
    HEXADECIMAL_UPPER,
    HEXADECIMAL_LOWER,
    OCTAL,

    POINTER,
};

/* echo '#include <stdlib.h>' | cpp -I/usr/include | grep <type> */

typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long int int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;

typedef unsigned int size_t;

typedef long int intptr_t;
typedef unsigned long int uintptr_t;

#endif