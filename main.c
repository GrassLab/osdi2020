#include <stdint.h>

#include "io.h"
#include "reset.h"
#include "uart.h"

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

void read_cmd(char *cmd) {
    char now;
    cmd[0] = 0;
    int now_cur = 0;
    while ((now = read_c()) != '\n') {
        if ((int)now == 240) {
            continue;
        } else if (now == 127) {  // delete
            now_cur -= 1;
            if (now_cur >= 0) {
                print_s("\b \b");
                now_cur -= 1;
            }
        } else {
            cmd[now_cur] = now;
            print_c(now);
        }
        now_cur++;
    }
    print_s("\n");
    cmd[now_cur] = 0;
}

void shell() {
    print_s("# ");
    char cmd[256];
    read_cmd(cmd);
    if (!strcmp(cmd, "help")) {
        print_s(
            "help      : print this help menu\n"
            "hello     : print Hello World!\n"
            "timestamp : print system timestamp\n"
            "reboot    : reboot the device\r\n");
    } else if (!strcmp(cmd, "hello")) {
        print_s("Hello World!\r\n");
    } else if (!strcmp(cmd, "timestamp")) {
        uint64_t frq, ct;
        asm volatile("mrs %0, cntpct_el0" : "=r"(ct));
        asm volatile("mrs %0, cntfrq_el0" : "=r"(frq));
        double frq_f = frq, ct_f = ct;
        print_d(ct_f / frq_f);
        print_s("\r\n");
    } else if (!strcmp(cmd, "reboot")) {
        reset(10);
    } else if (!strcmp(cmd, "clear")) {
        print_s("\033[2J\033[1;1H");
    } else {
        /* print_s("command not found: "); */
        /* print_s(cmd); */
        /* print_s("\r\n"); */
    }
}

int main() {
    // set up serial console
    uart_init();
    print_s("\033[2J\033[1;1H");

    while (1) {
        shell();
    }
}
