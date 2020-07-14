#include "uart.h"
#include "string.h"
#include "sd.h"
#include "fat.h"

extern unsigned long _end;

void main()
{
    uart_init();
    sd_init();
    
    fat_setup_mount();
    fat_lookup("AAA","TXT");
    fat_write_file();
    fat_read_file();


    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}