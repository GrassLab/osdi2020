#include "user_lib.h"
int main() {
    sys_uart_write("user1\n", 6);
    sys_uart_write("user2\n", 6);
}
