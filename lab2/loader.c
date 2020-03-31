#include "uart.h"
#include "libc.h"
#include "loader.h"

extern int __start;
extern int __end;

void loadimg() {
  uart_println("Start loading kernel image...");
  uart_println("Please input kernel load address(defualt: 0x80000):");
  uart_println("Please send kernel image from UART now:");
  /* The kernel image must have following protocal */
  /* recieve 'S' */
  /* recieve kernel size */
  /* recieve load address */
  /* start recieve kerenl_image based on kernel size */
  /* recieve 'E' */

  int kernel_size;

  /* default: UPLOAD_ADDRESS defined in loader.h */
  void *load_address = (void*)UPLOAD_ADDRESS;


  /* DATA_ACK */
  {
    char c = uart_getc();
    if (c != DATA_ACK)
      goto PROTOCAL_ERROR;
  }

  /* customize load address */
  {
    char c = uart_getc();
    if (c == ADDR_ACK) {
      load_address = (void *)uart_getul();
      uart_println("customize load address @ %x", load_address);
    }
  }

  /* kernel size */
  /* (e.g. 4000\r) */
  kernel_size = uart_getint();

  uart_println("Kernel Image Size: %d Load addr: %x", kernel_size, load_address);

  /* starting recieve kernel image */
  volatile char *kernel_addr = load_address;
  while(kernel_size--){
    *kernel_addr++ = uart_getc();
  }

  /* DATA_SYN */
  {
    char c = uart_getc();
    if (c != DATA_SYN)
      goto PROTOCAL_ERROR;
  }

  kentry_t kentry = (kentry_t)load_address;

  uart_println("adjust stack pointer to %x", load_address);
  asm volatile("mov sp, %0" :: "r"(load_address));

  uart_puts("Successfully loaded, executing uploaded kernel...\r\n");

  kentry();

 PROTOCAL_ERROR:
  uart_println("[ERR] loading image fault(protocal)");
}
