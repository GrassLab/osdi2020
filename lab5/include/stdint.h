#ifndef _STDINT_INCLUDE_H_
#define _STDINT_INCLUDE_H_

typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long long int int64_t;
#endif

/* Unsigned.  */
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
#ifndef __uint32_t_defined
typedef unsigned int uint32_t;
#define __uint32_t_defined
typedef unsigned long long int uint64_t;

#endif