#include "MiniUart.h"
#include "StringUtils.h"

#include <stddef.h>

#define BUFFER_MAX_SIZE 128u

static const char *kCommandStringList[] = {"help", "hello"};

static void printPrompt(void) {
    sendStringUART("> ");
}

static void readCommand(char *buffer) {
    size_t size = 0u;
    while (size < BUFFER_MAX_SIZE) {
        buffer[size] = recvUART();
        
        // echo back
        sendUART(buffer[size]);

        if (buffer[size++] == '\n') {
            break;
        }
    }
    buffer[size] = '\0';
}

static void help(void) {
    sendStringUART("Available commands:\n");
    for (size_t i = 0u; i < sizeof(kCommandStringList) / sizeof(const char *); ++i) {
        sendUART('\t');
        sendStringUART(kCommandStringList[i]);
        sendUART('\n');
    }
}

static void hello(void) {
    sendStringUART("Hello World!\n");
}

static void parseCommand(char *buffer) {
    // remove newline
    stripString(buffer);

    if (*buffer == '\0') {
        return;
    }

    if (compareString("help", buffer) == 0) {
        help();
    } else if (compareString("hello", buffer) == 0) {
        hello();
    } else {
        sendStringUART("command not found: ");
        sendStringUART(buffer);
        sendUART('\n');
    }
}

void shell(void) {
    char buffer[BUFFER_MAX_SIZE];
    size_t num;

    while (1) {
        printPrompt();

        readCommand(buffer);
        stripString(buffer);
        parseCommand(buffer);
    }
}
