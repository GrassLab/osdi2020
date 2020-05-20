#include "user_lib.h"
int main() {
    asm volatile("mov x0, 0xffffffff");
    asm volatile("ldr x1, [x0]");
    sys_uart_write("user1\n", 6);
    sys_uart_write("user2\n", 6);
}
