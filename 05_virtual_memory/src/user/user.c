#include "user_lib.h"

void test_write() {
    uart_write("12345\n", 6);
}

void main() {
    uart_write("User test\n", 10);
}
