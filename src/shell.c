/*
 * shell.c
 * Basic uart shell
 */

#include "types.h"
#include "uart.h"
#include "string.h"
#include "watchdog.h"
#include "mailbox.h"
#include "time.h"
#include "printf.h"

#define BUFFER_SIZE 1024
#define SHELL_CHAR "# " 

extern void enable_irq();

extern void init_irq();

void shell_splash()
{
    uart_puts("               _                            _                                     \r\n");
    uart_puts("              | |                          | |                                    \r\n");
    uart_puts(" __      _____| | ___ ___  _ __ ___   ___  | |__   ___  _ __ ___   ___            \r\n");
    uart_puts(" \\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\ | '_ \\ / _ \\| '_ ` _ \\ / _ \\\r\n");
    uart_puts("  \\ V  V /  __/ | (_| (_) | | | | | |  __/ | | | | (_) | | | | | |  __/          \r\n");
    uart_puts("   \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___| |_| |_|\\___/|_| |_| |_|\\___|   \r\n");
    uart_puts("\r\n");
    uart_puts("+-----------------------+-----------------------+\r\n");
    printf("|\tRevision\t|\t%08X\t|\r\n|\tVC start\t|\t%08X\t|\r\n", get_board_revision(), get_vc_memory());
    uart_puts("+-----------------------+-----------------------+\r\n");
}


void uart_read_line(char *buffer, size_t size)
{
    size_t position = 0;
    uint8_t c, e;

    while(position < size) {
        c = uart_getc();

        if (c == '\r' || c == '\n') {
            buffer[position++] = '\0';
            uart_puts("\r\n");
            return;
        } else if (c == 127) {
            if (position > 0) {
                buffer[--position] = 0;
                uart_puts("\b \b");
            }
        } else if (c == '[') {
            e = uart_getc();
            if (e == 'C' && position < strlen(buffer)) {
                uart_puts("\033[C");
                position++;
            } else if (e == 'D' && position > 0) {
                uart_puts("\033[D");
                position--;
            }
        } else if (c > 39 && c < 127) {
            buffer[position++] = c;
            uart_putc(c);
        }
    }
}

void shell_start() 
{
    uart_log(LOG_INFO, "Starting shell...");

    char line[BUFFER_SIZE];
    
    //char *cmd
    //char **args;

    char *shell_cmds[][2] = {
        {"hello", "useful command"},
        {"time", "get the current timestamp"},
        {"reboot", "restart the rpi, only works on real hardware"},
        {"shutdown", "halt the rpi"},
        {"loadker", "loads the kernel on the fly using uart"},
        {"exc", "issue a svc exception"},
        {"irq", "issue a irq exception"},
        {"help", "print this screen"}
    };

    shell_splash();

    while(true) {
        uart_puts("\r\n");
        uart_puts(SHELL_CHAR);
        uart_read_line(line, BUFFER_SIZE);

        if (!strcmp(line, shell_cmds[0][0])) {
            uart_puts("world");
        } else if (!strcmp(line, shell_cmds[1][0])) {
            printf("%f", get_time());
        } else if (!strcmp(line, shell_cmds[2][0])) {
            reboot();
        } else if (!strcmp(line, shell_cmds[3][0])) {
            shutdown();
        } else if (!strcmp(line, shell_cmds[4][0])){
            uart_puts("Waiting for the image...\r\n");
            int size = 0;
            for (size_t i = 0; i < 4; i++) {
                char c = uart_getc();
                size = size << 8;
                size += (int)c;
            }
            printf("Image size: %d\r\n", size);
            uart_puts("Loading the image...\r\n");
            uint8_t *kernel = (uint8_t *)0x10000;
            uint8_t *original_kernel = (uint8_t *)0x80000;
            while(--size){
                *kernel++ = uart_getc();
            }

            uart_puts("loading done\r\n");

            /* for (int i = 0; i < stmp; i++) {
                if (i % 100 == 0) printf("%d\r\n", i);
                *(original_kernel + i) = *(kernel - size + i);
            } */

            uart_puts("copy done \r\n");
            uart_puts("Booting...\r\n");
            void (*boot)(void) = 0x10000;
            boot();
        } else if (!strcmp(line, shell_cmds[5][0])){
            asm volatile("svc #1");
            //asm volatile("brk #1");
        } else if (!strcmp(line, shell_cmds[6][0])){
            //init_irq();
            //enable_irq();
            //local_timer_init();
            //core_timer_enable();
            timed_delay();
        } else if (!strcmp(line, shell_cmds[7][0])) {
            for(size_t i = 0; i < sizeof(shell_cmds) / sizeof(shell_cmds[0]); i++)
                printf("%s : %s \r\n", shell_cmds[i][0], shell_cmds[i][1]);
       } else {
            printf("ERROR: %s unknown command, seek <help>", line);
        }
    }
}
