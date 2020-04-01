#include "uart.h"
#include "reset.h"


int my_strcmp(char * str1, char * str2);
int cmd_box(char * command);
void process_cmd(char * command);
void print_hello();
int get_cpu_time();
int get_cpu_clock();
void get_timestamp();
void load_image();
char * int2char(int input);
void copy_and_jump_to_kernel();
void copy_current_kernel_and_jump(char *new_address);