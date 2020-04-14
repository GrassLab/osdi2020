#include "uart.h"
#include "ftoa.h"

char read_c();
char read_b();
int read_i();
unsigned long long int read_h();
void print_s(char *ch);
void print_c(char ch);
void print_i(int x);
void print_h(int x);
void print_d(double x);
