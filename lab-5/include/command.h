#ifndef COMMAND_H
#define COMMAND_H

void interative();
void command_help();
void command_reboot();
void command_timestamp();
void command_hardware_info();
void command_load_image();
void command_version();
void command_picture();
void command_get_uart_clock();
void command_exc();
void command_irq();

typedef struct command {
    char name[32];
    char description[512];
    void (*function)();
} Command;

#endif