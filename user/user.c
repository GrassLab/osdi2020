#include "user_lib.h"
char cmd[200];

int strcmp(const char *p1, const char *p2) {
    const unsigned char *s1 = (const unsigned char *)p1;
    const unsigned char *s2 = (const unsigned char *)p2;
    unsigned char c1, c2;
    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0') return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

void my_strlen(char *str, int *len) {
    for (*len = 0; str[*len]; (*len)++)
        ;
}

void print_s(char *string) {
    int len;
    my_strlen(string, &len);
    for (int i = 0; i < 100000; i++) {
        if (string[i] != 0) {
            len++;
        }
        break;
    }
    sys_uart_write(string, len);
}

void print_i(int x) {
    char tmp[2];
    if (x < 0) {
        print_s("-");
        x = -x;
    }
    if (x >= 10) print_i(x / 10);
    tmp[0] = x % 10 + '0';
    sys_uart_write(tmp, 1);
}

void read_cmd() {
    char now;
    int now_cur = 0;
    while (1) {
        now = sys_uart_getc();
        if (now == '\n') break;
        if (now == 127) {  // delete
            now_cur -= 1;
            if (now_cur >= 0) {
                print_s("\b \b");
                now_cur -= 1;
            }
        } else {
            cmd[now_cur] = now;
            sys_uart_write(&now, 1);
        }
        now_cur++;
    }
    print_s("\n");
    cmd[now_cur] = 0;
}

void shell() {
    print_s("# ");
    read_cmd();
    if (!strcmp(cmd, "help")) {
        print_s(
            "help      : print this help menu\n"
            "page      : raise a page fault\n"
            "hello     : print Hello World!\n");
    } else if (!strcmp(cmd, "hello")) {
        print_s("Hello World!\r\n");
    } else if (!strcmp(cmd, "page")) {
        asm volatile("mov x0, 0xffffffff");
        asm volatile("ldr x1, [x0]");
    } else if (!strcmp(cmd, "fork")) {
        int pid = sys_fork();
        print_s("Child pid: ");
        asm volatile("pid:");
        print_i(pid + 10);
        print_s("\n");
    } else if (!strcmp(cmd, "clear")) {
        print_s("\033[2J\033[1;1H");
    } else {
    }
}

int main() {
    while (1) {
        shell();
    }
}
