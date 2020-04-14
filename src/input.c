#include "ftoa.h"
#include "input.h"
#include "uart.h"

unsigned long long int hex2int(char *hex) {
    unsigned long long int val = 0;
    while (*hex) {
        // get current character then increment
        unsigned long long int byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the
        // ascii table indexes
        if (byte >= '0' && byte <= '9')
            byte = byte - '0';
        else if (byte >= 'a' && byte <= 'f')
            byte = byte - 'a' + 10;
        else if (byte >= 'A' && byte <= 'F')
            byte = byte - 'A' + 10;
        // shift 4 to make space for new digit, and add the 4 bits of the new
        // digit
        val = (val << 4) | (byte & 0xF);
    }
    return val;
}

int check_digit(char ch) { return (ch >= '0') && (ch <= '9'); }

char read_c() { return uart_getc(); }
char read_b() { return uart_getb(); }

char *read_s(char *str, int max_size) {
    for (int i = 0; i < max_size; i++) {
        str[i] = read_c();
        print("%c", str[i]);
        if (str[i] == 127) {  // delete
            i--;
            if (i >= 0) {
                print("\b \b");
                i--;
            }
        }
        if (str[i] == '\n' || str[i] == '\r') {
            str[i] = 0;
            break;
        }
    }
    str[max_size - 1] = 0;
    return str;
}

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

unsigned long long int read_h() {
    char str[50];
    char *value = read_s(str, 50);
    return hex2int(value + 2);
}
