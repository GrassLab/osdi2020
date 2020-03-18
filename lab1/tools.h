#include "uart.h"
#include "reset.h"

int my_strcmp(char * str1, char * str2);
int cmd_box(char * command);
void process_cmd(char * command);
void print_hello();
unsigned long get_cpu_time();
unsigned long get_cpu_clock();
void get_timestamp();