#include "lib.h"

void strncmp(const char *cs, const char *ct, int len)
{
    unsigned char c1, c2;
    while (len--) {
        c1 = *cs++;
        c2 = *ct++;
        if (c1 != c2) return c1 < c2 ? -1 : 1;
        if (!c1) break;
    }
}
// void shell()
// {
//     call_sys_uart_write("Staring shell...\n");
//     while (1) { 
//         // get user input
//         char user_input[256];
//         char tmp;
//         int i = 0;
//         uart_send('>');
//         while (i < 10) {
//             tmp = uart_getc();
//             if (tmp == '\n') break;
//             uart_send(tmp);
//             user_input[i++] = tmp;
//         }
//         user_input[i] = '\0';
//         call_sys_uart_write("\n");
//         if (i == 0) {
//             continue;
//         }
//         /*
//          ** <hello> Echo hello
//          */
//         if (strncmp(user_input, "hello", 5) == 0) {
//             call_sys_uart_write("Hello World!\n");
//         }
//         /*
//          ** <help> list the existed commands
//          */
//         else if (strncmp(user_input, "help", 4) == 0) {
//             call_sys_uart_write("hello: print hello world.\n");
//             call_sys_uart_write("help: help.\n");
//         }
//         /*
//         ** Invalid command
//         */
//         else {
//             call_sys_uart_write("Error: command ");
//             call_sys_uart_write(user_input);
//             call_sys_uart_write(" not found, try <help>.\n");
//         }
//     }
// }

void main(){
	// call_sys_uart_write("Welcome to UUUUser.\n");
    call_sys_get_taskid();
	while(1);
	// shell();
}