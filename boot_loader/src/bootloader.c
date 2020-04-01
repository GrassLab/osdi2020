#include "uart.h"
#include "utils.h"

void loadImage()
{
    char *kernel = (char *)0x80000;
    char buf[8];
    int buf_ptr = 0;
    int size;

again:
    uartPuts("start to load kernel...\n");

    while (1)
    {
        char in_c = uartGetc();

        if (in_c != '\n')
        {
            buf[buf_ptr++] = in_c;
        }
        else
        {
            buf[buf_ptr] = '\0';
            break;
        }
    }

    size = atoi(buf);

    if (size > 1024 * 1024 * 1024)
    {
        // size error
        uartPuts("kernel size error!");
        goto again;
    }

    uartPuts("kernel image size: ");
    printInt(size);
    uartPuts("      load addr : 0x80000\n");

    // read the kernel
    while (size--)
        *kernel++ = uartGetc();

    asm volatile(
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret");
}