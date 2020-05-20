#include "user_lib.h"
#include "my_string.h"

void printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    char buf[64];
    unsigned int len = my_vsprintf(buf, fmt, args);
    uart_write(buf, len);
}
