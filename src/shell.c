#include "type.h"
#include "utils.h"
#include "device/time.h"
#include "device/power.h"
#include "device/mbox.h"
#include "device/uart.h"
#include "interrupt/timer.h"

const static uint32_t MAX_BUFFER_SIZE = 512;

static inline void helpCmd()
{
    uartPuts("Commands:\n");
    uartPuts("    help               print all available commands\n");
    uartPuts("    hello              print Hello World!\n");
    uartPuts("    timestamp          get current timestamp\n");
    uartPuts("    reboot             reboot rpi3\n");
    uartPuts("    hardware           print hardware information\n");
    uartPuts("    exc                supervisor call #1\n");
    uartPuts("    irq                enable core timer\n");
}

static inline void helloCmd()
{
    uartPuts("Hello World!\n");
}

static inline void timestampCmd()
{
    double t = getTime();

    uartPuts("[");
    uartFloat(t);
    uartPuts("]\n");
}

static inline void rebootCmd()
{
    uartPuts("-------Rebooting--------\n\n");

    reset(10);
}

static inline void hardwareInfoCmd()
{
    getBoardRevision();
    getVCMemory();
}

static inline void excCmd()
{
    asm volatile("svc #1");
}

static inline void irqCmd()
{
    asm volatile("svc #2");
}

static inline void noneCmd(const char *input)
{
    uartPuts("Unknown command: ");
    uartPuts(input);
    uartPuts("\n");
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
    else if (strcmp(input, "exc"))
        return exc;
    else if (strcmp(input, "irq"))
        return irq;

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
    case exc:
        excCmd();
        break;
    case irq:
        irqCmd();
        break;
    default:
        noneCmd(input);
        break;
    }
}

static inline void helloMessage()
{
    uartPuts("\n|------------------------|\n");
    uartPuts("| Welcome to OSDI shell! |\n");
    uartPuts("|------------------------|\n\n");
    uartPuts("> ");
}

void runShell()
{
    char buf[MAX_BUFFER_SIZE];
    uint32_t buf_ptr = 0;

    helloMessage();

    // echo everything back
    while (1)
    {
        char in_c = uartGetc();
        uartSend(in_c);

        if (in_c == '\n')
        {
            buf[buf_ptr] = '\0';

            processCmd(buf);
            uartPuts("> ");

            buf_ptr = 0;
            buf[buf_ptr] = '\0';
        }
        else
        {
            buf[buf_ptr++] = in_c;
        }
    }
}