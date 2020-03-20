 #include "uart.h"

 void kernel_main(void) {
     uart_init();

     // Wait for user input before sending the Hello world
     uart_recv();
     uart_send_string("Hello world!\r\n");

     while (1) {
         uart_send(uart_recv());
     }
 }