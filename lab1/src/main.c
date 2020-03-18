#include "uart.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int *)(0x3F10001c))
#define PM_WDOG ((volatile unsigned int *)(0x3F100024))

void reset(int tick) {               // reboot after watchdog timer expire
    *PM_RSTC = (PM_PASSWORD | 0x20); // full reset
    *PM_WDOG = (PM_PASSWORD | tick); // number of watchdog tick
}

void cancel_reset() {
    *PM_RSTC = (PM_PASSWORD | 0); // full reset
    *PM_WDOG = (PM_PASSWORD | 0); // number of watchdog tick
}

int strcmp(char *str1, char *str2, int n) {
    for (int i = 0; i < n; i++) {
        if (str1[i] != str2[i]) {
            return 0;
        }
    }
    return 1;
}

void memset(char *str, char c, int n) {
    for (int i = 0; i < n; i++) {
        str[i] = c;
    }
}

void Hello() {
    uart_puts("Hello World!\n");
}

void Help() {
    uart_puts("hello: Hello World!\n");
    uart_puts("timestamp: get current time!\n");
    uart_puts("reboot: reboot rpi3!\n");
    uart_puts("help: help!\n");
}

unsigned long get_cntfrq() {
    unsigned long cntfrq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(cntfrq));
    return cntfrq;
}

unsigned long get_cntpct() {
    unsigned long cntpct;
    asm volatile("mrs %0, cntpct_el0" : "=r"(cntpct));
    return cntpct;
}

void ul2s(char *str, unsigned long l) {
    int i, k, n;
    char tmp_str[33];
    memset(tmp_str, '\0', 33);

    for (i = 0; i < 33; i++) {
        if (l == 0) {
            break;
        }
        n = l % 10;
        tmp_str[i] = n + '0';
        l = l / 10;
    }

    for (k = 0; k < i; k++) {
        str[k] = tmp_str[i - 1 - k];
    }
    return;
}

void Timestamp(char *str, float frq) {
    float time_cnt = get_cntpct();
    memset(str, '\0', 200);
    ul2s(str, time_cnt);
    uart_puts(str);

    uart_puts("/");

    memset(str, '\0', 200);
    ul2s(str, frq);
    uart_puts(str);
    uart_puts("\n");
    return;
}

void main() {
    // *set up serial console
    uart_init();
    unsigned long frq = get_cntfrq();

    char str[200];
    int i = 0;
    memset(str, '\0', 200);
    uart_getc();
    uart_puts("MACHINE IS OPEN!!\n");
    uart_puts("# ");

    while (1) {
        char c = uart_getc();
        str[i] = c;
        uart_send(c);
        if (c == '\n') {
            if (i == 5 && strcmp(str, "hello", i)) {
                Hello();
            } else if (i == 4 && strcmp(str, "help", i)) {
                Help();
            } else if (i == 9 && strcmp(str, "timestamp", i)) {
                Timestamp(str, frq);
            } else if (i == 6 && strcmp(str, "reboot", i)) {
                reset(6);
            } else {
                uart_puts("Err: ");
                str[i] = '\0';
                uart_puts(str);
                uart_puts(" command not found, try <help>\n");
            }
            i = 0;
            memset(str, '\0', 200);
            uart_puts("# ");
        } else {
            i++;
        }
    }
    return;
}
