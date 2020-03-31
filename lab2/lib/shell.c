#include "MiniUart.h"
#include "StringUtils.h"
#include "utils.h"

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

static void loadimg(void) {
    static char * const kLoadAddr = (char *)0x40000;
    static const size_t kImgMaxSize = 0x40000;

    sendStringUART("Please input kernel image size (max: 262144): ");

    char buffer[BUFFER_MAX_SIZE];
    readCommand(buffer);
    stripString(buffer);

    size_t img_size = getIntegerFromString(buffer);
    if (img_size > kImgMaxSize) {
        sendStringUART("[LOG][ERROR] Over the capacity.\n");
        return;
    }
    sendHexUART(img_size);
    sendUART('\n');

    sendStringUART("Start loading os kernel image ...\n");

    size_t current_size = 0u;
    while (current_size < img_size) {
        kLoadAddr[current_size++] = recvUART();
    }
    branchAddr(kLoadAddr);
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
    } else if (compareString("loadimg", buffer) == 0) {
        loadimg();
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
