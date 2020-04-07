#include "type.h"
#include "uart.h"
#include "bootloader.h"
#include "utils.h"

void processCmd(const char *input)
{
    if (input[0] == '\0')
        return;

    if (strcmp(input, "loadimg"))
        loadImage();
    else
        uartPuts("Please type loadimg to start loading kernel image!\n");
}

void runShell()
{
    char buf[32];
    int buf_ptr = 0;

    uartPuts("Please type loadimg to start loading kernel image!\n");
    uartPuts("> ");

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