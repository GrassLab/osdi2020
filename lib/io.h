#ifndef __LIB_IO_H
#define __LIB_IO_H

#include "type.h"

typedef struct {
    int is_zero_padding;
    int signed_flag;
    int int_length;
    int after_point_length;
    int is_justify_left;

    enum type_t type;
} format_t;

int printf ( const char * format, ... );
char * gets ( char * str );
format_t parse_format ( const char ** input );


#endif