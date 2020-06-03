#include "loadimg.h"
#define  TMP_KERNEL_ADDR  0xB00000
#define  NEW_KERNEL_ADDR  0x81000

char *old_start, *old_end, *new_start, *new_end;
extern char _start, _end;
int size = 0, addr = 0;

int overlap(char *ostart, char *oend, char *nstart, char *nend){
  if ((ostart - nend) * (oend - nstart) <= 0)
    return 1;
  else
    return 0;
}

void jump() {
  uart_puts("start loading!\n");
  uart_puts("|>");
  // read the kernel
  for (int num = 0; num < size; num++) {
    *(new_start + num) = uart_getb();
    if (num % (size/20) == 0) {
      uart_puts("\b=>");
    }
  }
  uart_puts("\nready to jump!\n");
  // restore arguments and jump to the new kernel.
  asm volatile("br %0" :: "r"((char *)(addr)));
}

void copyjump() {
  if (overlap(old_start, old_end, new_start, new_end) == 1) { //Overlap
    uart_puts("overlap!!\n");
    char *tmp_addr = (char *)(TMP_KERNEL_ADDR);
    for (int i = 0; i < old_end - old_start; i++) {
      *(tmp_addr + i) = *(char *)(old_start + i);
    }
    void (*copy_func_ptr)() = jump;
    char *current_addr = (char *)copy_func_ptr - old_start + tmp_addr;
    asm volatile("br %0" :: "r"((char *)(current_addr)));
  }
}

void loadimg() {
  char buf[30];
  // say hello. To reduce Gloader size I removed uart_puts()
  uart_send('R');
  uart_send('B');
  uart_send('I');
  uart_send('N');
  uart_send('6');
  uart_send('4');
  uart_send('\r');
  uart_send('\n');

  // read the kernel's size
  char chr;
  while((chr = uart_getc()) != '\n') {
    size *= 10;
    size += (chr - '0');
  }

  // send negative or positive acknowledge
  if(size<64 || size>1024*1024) {
    // size error
    uart_send('S');
    uart_send('E');
    return;
  }
  uart_send('O');
  uart_send('K');
  uart_send('\n');

  // read the kernel'saddress
  while((chr = uart_getc()) != '\n') {
    addr *= 10;
    addr += (chr - '0');
  }
  new_start = (char *)(addr);
  new_end = new_start + size;

  old_start = (char*)&_start;
  old_end = (char*)&_end;

  uart_puts("Old Start: ");
  uart_puts(ftoa((int)old_start, buf, 5));
  uart_puts("\n");
  uart_puts("Old End: ");
  uart_puts(ftoa((int)old_end, buf, 5));
  uart_puts("\n");
  uart_puts("New Start: ");
  uart_puts(ftoa((int)new_start, buf, 5));
  uart_puts("\n");

  copyjump();
  jump();
}
