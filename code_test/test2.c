#include "lib.h"

void delay(unsigned long num)
{
    for (unsigned long i = 0; i < num; i++)
    {
        asm volatile("nop");
    }
}

#define BUFFER_SIZE 64
int main()
{
    char *str = "\n=====\nuser program 2 test\n=====\n";
    uart_puts(str);
    while(1){
        uart_send('a');
        delay(1000000);
    }

}
