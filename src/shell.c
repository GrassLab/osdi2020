/*
 * shell.c
 * Basic uart shell
 */

#include "types.h"
#include "uart.h"
#include "string.h"
#include "watchdog.h"

#define BUFFER_SIZE 1024
#define SHELL_CHAR "# "

void shell_start() 
{
    char line[BUFFER_SIZE];
    char int_buffer[11];
    
    //char *cmd
    //char **args;

    char *shell_cmds[][2] = {
        {"hello", "useful command"},
        {"time", "get the current timestamp"},
        {"reboot", "restart the rpi, only works on real hardware"},
        {"shutdown", "halt the rpi"},
        {"loadker", "loads the kernel on the fly using uart"},
        {"help", "print this screen"}
    };

    while(true) {
        uart_puts("\r\n");
        uart_puts(SHELL_CHAR);
        uart_read_line(line, BUFFER_SIZE);
        if (!strcmp(line, shell_cmds[0][0])) {
            uart_puts("world");
        } else if (!strcmp(line, shell_cmds[1][0])) {
            ftoa(mm_ticks()/(float)mm_freq(), int_buffer);
            uart_puts(int_buffer);
        } else if (!strcmp(line, shell_cmds[2][0])) {
            reboot();
        } else if (!strcmp(line, shell_cmds[3][0])) {
            shutdown();
        } else if (!strcmp(line, shell_cmds[4][0])){
            uart_puts("Waiting for the image...\r\n");
            uint32_t size = uart_getc();
            size|=uart_getc()<<8;
            size|=uart_getc()<<16;
            size|=uart_getc()<<24;

            uart_putc((size>>24)&0xff);
            uart_putc((size>>16)&0xff);
            uart_putc((size>>8)&0xff);
            uart_putc(size&0xff);
            
            uart_puts("Loading the image...\r\n");
            uint8_t * kernel = (uint8_t *)0x80000;
            while(size-- > 0){
                *kernel++ = uart_getc();
            }
            uart_puts("Booting...\r\n");
            void (*boot)(void) = 0x80000;
            boot();
        } else if (!strcmp(line, shell_cmds[5][0])) {
            for(size_t i = 0; i < sizeof(shell_cmds)/sizeof(shell_cmds[0]); i++) {
                uart_puts(shell_cmds[i][0]);
                uart_puts(" : ");
                uart_puts(shell_cmds[i][1]);
                uart_puts("\r\n");
            }
       } else {
            uart_puts("ERROR: ");
            uart_puts(line);
            uart_puts(" unknown command, seek <help>");
        }
    }
}
