#include "type.h"
#include "cmd.h"
#include "uart.h"
#include "time.h"

bool strcmp(const char* s1, const char* s2)
{
	for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++) {
		if ((*s1 - *s2) != 0)
			return false;
	}

    if (*s1 == '\0' && *s2 == '\0')
	{
		return true;
	}
    else {
        return false;
    }
}

void itoaAndPrint(int t) {
    char buf[1024];
    int buf_ptr = 0; 
    while (t > 0) {
        buf[buf_ptr++] = (t % 10) + '0';
        t = t / 10;
    }
    buf[buf_ptr] = '\0';
    for (int i = buf_ptr-1, j = 0, half = (buf_ptr-1)/2; i > half; --i, ++j) {
        char tmp = buf[i];
        buf[i] = buf[j];
        buf[j] = tmp;
    }

    uart_puts(buf);
}

command_t checkCmdType(const char *input) {
    if (strcmp(input, "help")) 
        return help;
    else if (strcmp(input, "hello"))
        return hello;
    else if (strcmp(input, "timestamp"))
        return timestamp;

    return none;
}

void helpCmd() {
    uart_puts("Commands:\n");
    uart_puts("    help          print all available commands\n");
    uart_puts("    hello         print Hello World!\n");
    uart_puts("    timestamp     get current timestamp\n");
}

void helloCmd() {
    uart_puts("Hello World!\n");
}

void timestampCmd() {
    double t = getTime();
    uart_puts("[");
    
    int i = (int)t;
    int frac = (int)((t - (double)i) * 1000000);
    itoaAndPrint(i);
    uart_puts(".");
    itoaAndPrint(frac);
    
    uart_puts("]\n");
}

void noneCmd(const char* input) {
    uart_puts("Unknown command: ");
    uart_puts(input);
    uart_puts("\n");
}

void processCmd(const char* input) {
    if (input[0] == '\0')
        return;

    switch (checkCmdType(input)) {
        case help: 
            helpCmd();
            break;
        case hello:
            helloCmd();
            break;
        case timestamp:
            timestampCmd();
            break;
        default:
            noneCmd(input);
            break;
    }
}