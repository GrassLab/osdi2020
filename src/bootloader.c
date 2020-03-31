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

    // send negative or positive acknowledge
    if (size < 64 || size > 1024 * 1024)
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
        *kernel++ = uartGetcWithCR();

    // restore arguments and jump to the new kernel.
    asm volatile(
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret");
}