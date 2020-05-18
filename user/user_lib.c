#include "user_lib.h"
#include "my_string.h"

void printf(char* fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    char str[1024];
    unsigned int len = my_vsprintf(str, fmt, args);
    uart_write(str, len);
}
