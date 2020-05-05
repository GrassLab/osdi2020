#include "shell.h"
#include "uart0.h"
#include "frame_buffer.h"
#include "mbox.h"
#include "util.h"
#include "mm.h"
#include "schedule.h"

#define CMD_LEN 128

void kernel_main() {
    // Initialize UART
    uart_init();
    uart_flush();
    uart_printf("\n[%f] Init PL011 UART done", get_timestamp());

    // Initialize Frame Buffer
    fb_init();
    uart_printf("\n[%f] Init Frame Buffer done", get_timestamp());

    // Welcome Messages
    // fb_splash();
    uart_printf("\n\n _  _  ___ _____ _   _  ___  ___ ___ ___ \n");
    uart_printf("| \\| |/ __|_   _| | | |/ _ \\/ __|   \\_ _|\n");
    uart_printf("| .` | (__  | | | |_| | (_) \\__ \\ |) | | \n");
    uart_printf("|_|\\_|\\___| |_|  \\___/ \\___/|___/___/___|\n\n");
    mbox_board_revision();
    mbox_vc_memory();
    uart_printf("\n");

    mm_init();
    schedule_init();

    while (1) {
        uart_printf("idle task!\n");
        for (int i = 0; i < 1000000; i++);
        schedule();
    }
}

int main() {
    while (1) {
        char cmd[CMD_LEN];
        shell_input(cmd);
        shell_controller(cmd);
    }
}