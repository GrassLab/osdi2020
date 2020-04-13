#include "exec.h"
#include "timer.h"

struct cmd
{
    char name[15];
    char description[40];
    void(*callback)(void);
};

struct cmd cmd_list[] = { 
    { .name = "hello", .description = " Show \"Hello World!\"", .callback = exec_hello}, 
    { .name = "help", .description =  " Show commands available.", .callback = exec_help}, 
    { .name = "boot", .description = " Reboot device.", .callback = exec_boot}, 
    { .name = "timestamp", .description = " Get current timestamp.", .callback = exec_timestamp}, 
    { .name = "send_kernel", .description = " send kernel img to raspi", .callback = exec_send_kernel}, 
    { .name = "exc", .description = " go to kernel space", .callback = exec_exc},
    { .name = "irq", .description = " timer interrupt", .callback = exec_irq},
    { .name = "end", .description = " end timer interrupt", .callback = exec_end},
};


void exec_exc(){
    _print("exec svc \n");
    asm volatile ("svc #1");
    // asm volatile ("brk #1");
}

void exec_irq(){
    asm volatile ("svc #2");
}

void exec_end(){
    asm volatile ("svc #3");
}

void exec_timestamp(){
    asm volatile ("svc #4");
}

void exec_send_kernel(){
    copy_self_kernel();
}

void exec_boot(){
    uart_puts("rebooting\n");
	reset(100);
	while(1);
}


void exec_hello(){
    uart_puts("Hello World!\n");
}

void exec_help(){
    for(int i = 0; i < CMD_NUM; i++) {
        uart_puts(cmd_list[i].name);
        uart_send(':');
        uart_puts(cmd_list[i].description);
        uart_puts("\n");
    }
}

int _compare_input(char * input){
    for(int i = 0; i < CMD_NUM; i++) {
        if(!_strcmp(input,cmd_list[i].name)){
            cmd_list[i].callback();
            return 1;
        }
    }
    return 0;
}