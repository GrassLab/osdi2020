
#include "ftoa.h"
#include "uart.h"

int check_digit(char ch) { return (ch >= '0') && (ch <= '9'); }

char read_c() { return uart_getc(); }

int read_i() {
    int x = 0, f = 0;
    char ch = 0;
    while (!check_digit(ch)) {
        f |= ch == '-';
        ch = read_c();
    }
    while (check_digit(ch)) x = (x << 3) + (x << 1) + (ch ^ 48), ch = read_c();
    return f ? -x : x;
}

void print_s(char *ch) { uart_puts(ch); }

void print_c(char ch) { uart_send(ch); }

void print_i(int x) {
    if (x < 0) {
        print_c('-');
        x = -x;
    }
    if (x >= 10) print_i(x / 10);
    print_c(x % 10 + '0');
}

void print_d(double x) {
    char ans[30];
    print_s(ftoa(x, ans, 5));
}
