#include "type.h"
#include "uart.h"
#include "time.h"
#include "power.h"
#include "mbox.h"

const static unsigned int MAX_BUFFER_SIZE = 512;

bool strcmp(const char *s1, const char *s2)
{
    for (; *s1 != '\0' && *s2 != '\0'; s1++, s2++)
    {
        if ((*s1 - *s2) != 0)
            return false;
    }

    if (*s1 == '\0' && *s2 == '\0')
    {
        return true;
    }
    else
    {
        return false;
    }
}

void itoaAndPrint(int i)
{
    char buf[MAX_BUFFER_SIZE];
    int buf_ptr = 0;
    while (i > 0)
    {
        buf[buf_ptr++] = (i % 10) + '0';
        i = i / 10;
    }
    buf[buf_ptr] = '\0';
    for (int e = buf_ptr - 1, s = 0, half = (buf_ptr - 1) / 2; e > half; --e, ++s)
    {
        char tmp = buf[s];
        buf[s] = buf[e];
        buf[e] = tmp;
    }

    uart_puts(buf);
}

void ftoaAndPrint(double f)
{
    int i = (int)f;
    int frac = (int)((f - (double)i) * 100000);
    itoaAndPrint(i);
    uart_puts(".");
    itoaAndPrint(frac);
}

static inline void helpCmd()
{
    uart_puts("Commands:\n");
    uart_puts("    help               print all available commands\n");
    uart_puts("    hello              print Hello World!\n");
    uart_puts("    timestamp          get current timestamp\n");
    uart_puts("    reboot             reboot rpi3\n");
}

static inline void helloCmd()
{
    uart_puts("Hello World!\n");
}

static inline void timestampCmd()
{
    double t = getTime();

    uart_puts("[");
    ftoaAndPrint(t);
    uart_puts("]\n");
}

static inline void rebootCmd()
{
    uart_puts("-------Rebooting--------\n\n");

    reset(10);
}

static inline void hardwareInfoCmd()
{
    getBoardRevision();
    getVCMemory();
}

static inline void noneCmd(const char *input)
{
    uart_puts("Unknown command: ");
    uart_puts(input);
    uart_puts("\n");
}

command_t checkCmdType(const char *input)
{
    if (strcmp(input, "help"))
        return help;
    else if (strcmp(input, "hello"))
        return hello;
    else if (strcmp(input, "timestamp"))
        return timestamp;
    else if (strcmp(input, "reboot"))
        return reboot;
    else if (strcmp(input, "hardware"))
        return hardwareInfo;

    return none;
}

void processCmd(const char *input)
{
    if (input[0] == '\0')
        return;

    switch (checkCmdType(input))
    {
    case help:
        helpCmd();
        break;
    case hello:
        helloCmd();
        break;
    case timestamp:
        timestampCmd();
        break;
    case reboot:
        rebootCmd();
        break;
    case hardwareInfo:
        hardwareInfoCmd();
        break;
    default:
        noneCmd(input);
        break;
    }
}

static inline void helloMessage()
{
    uart_puts("\n|------------------------|\n");
    uart_puts("| Welcome to OSDI shell! |\n");
    uart_puts("|------------------------|\n\n");
    uart_puts("> ");
}

void runShell()
{
    char buf[MAX_BUFFER_SIZE];
    int buf_ptr = 0;

    helloMessage();

    // echo everything back
    while (1)
    {
        char in_c = uart_getc();
        uart_send(in_c);

        if (in_c == '\n')
        {
            buf[buf_ptr] = '\0';

            processCmd(buf);
            uart_puts("> ");

            buf_ptr = 0;
            buf[buf_ptr] = '\0';
        }
        else
        {
            buf[buf_ptr++] = in_c;
        }
    }
}