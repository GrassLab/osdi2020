#include "uart.h"
#include "mystd.h"
#include "vfs.h"
#include "fat32.h"

void assert(bool res){
    if(res) uart_puts("[assert] success\n");
    else uart_puts("[assert] fail\n");
}

void main()
{
    uart_init();
    

    init_fat32();

    struct file_descriptor* fd = vfs_open("hello", 0);
    fd = vfs_open("TEST.TXT", 0);

    fd_lseek(fd, 0);
    vfs_write(fd, "Hello OSDI !", 12);

    char buf[128] = {0};
    vfs_read(fd, buf, 100);
    uart_puts(buf);
    uart_puts("\n");

    fd_lseek(fd, 0);
    vfs_write(fd, "Bye  ", 5);

    vfs_read(fd, buf, 100);
    uart_puts(buf);
    uart_puts("\n");


}
