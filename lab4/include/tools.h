#include "peripherals/uart.h"
#include "peripherals/timer.h"
#include "reset.h"
#include "irq.h"
#include "shed.h"

int my_strcmp(char * str1, char * str2);
char *itoa(int num, char *str, int base);
int atoi(const char *str);
int cmd_box(char * command);
void process_cmd(char * command);
void print_hello();
int get_cpu_time();
int get_cpu_clock();
void get_timestamp();
void load_image();
void get_number();
char * int2char(int input);
void copy_and_jump_to_kernel(char *new_address, int kernel_size); 
void copy_kernel_and_load_images(char *new_address, int kernel_size);


extern int get_el ();
extern void delay (unsigned long);
