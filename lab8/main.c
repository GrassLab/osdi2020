#include "fat.h"
#include "mm.h"
#include "system.h"
#include "tmpfs.h"
#include "uart.h"
#include "util.h"
#include "vfs.h"

void main()
{
    uart_init();
    fat32_mount();

    fat32_lookup(fat32_root, "file", ((void*)0));
    uart_puts("\n");
    fat32_readfile(fat32_root, "START.ELF");

    char buf[32] = { 0 };
    char wbuf[16] = { 0 };
    int len = 10;
    int rvalue = 0;
    //const char test_filename[] = "TEST.TXT";
    const char test_filename[] = "START.ELF";

    uart_puts("Open ");
    uart_puts(test_filename);
    uart_puts(" and read 3 char:\n\r");
    rvalue = fat32_read(fat32_root, test_filename, buf, 0, len);
    buf[len] = '\0';
    if (rvalue == 0) {
        uart_puts("Read From file: ");
        uart_puts(buf);
        uart_puts("\r\n");
    }
    uart_puts("===\n\r");

    uart_puts("Open ");
    uart_puts(test_filename);
    uart_puts(" and write 3 char:\n\r");
    wbuf[0] = 'A';
    wbuf[1] = 'A';
    wbuf[2] = 'A';
    rvalue = fat32_write(fat32_root, test_filename, wbuf, 0, 3);
    wbuf[3] = '\0';
    if (rvalue == 0) {
        uart_puts("Write to file\n\r");
    }
    uart_puts("===\n\r");

    uart_puts("Open ");
    uart_puts(test_filename);
    uart_puts(" and read 3 char:\n\r");
    fat32_read(fat32_root, test_filename, buf, 0, len);
    buf[len] = '\0';
    if (rvalue == 0) {
        uart_puts("Read From file: ");
        uart_puts(buf);
        uart_puts("\r\n");
    }
}
