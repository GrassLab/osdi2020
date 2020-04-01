#include "loadimg.h"
#define  TMP_KERNEL_ADDR  0x100000
#define  NEW_KERNEL_ADDR  0x90000

char *old_start, *old_end, *new_start, *new_end;
extern char _start, _end;

int overlap(char *ostart, char *oend, char *nstart, char *nend){
  if ((ostart - nend) * (oend - nstart) < 0)
    return 1;
  else
    return 0;
}

void loadimg() {
  int size = 0, addr = 0;
  //char *kernel=(char*)0x80000;
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
  /*
  size=uart_getc();
  size|=uart_getc()<<8;
  size|=uart_getc()<<16;
  size|=uart_getc()<<24;
  size|=uart_getc()<<32;
  */
  size+=uart_getc();
  size*=10;
  size+=uart_getc();
  size*=10;
  size+=uart_getc();
  size*=10;
  size+=uart_getc();
  size*=10;
  size+=uart_getc();

  char buf[30];
  uart_puts(ftoa(size, buf, 5));
  uart_puts("\n");
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
  /*
  addr=uart_getc();
  addr|=uart_getc()<<8;
  addr|=uart_getc()<<16;
  addr|=uart_getc()<<24;
  addr|=uart_getc()<<32;
  */
  addr+=uart_getc();
  addr*=10;
  addr+=uart_getc();
  addr*=10;
  addr+=uart_getc();
  addr*=10;
  addr+=uart_getc();
  addr*=10;
  addr+=uart_getc();
  uart_puts(ftoa(addr, buf, 5));
  uart_puts("\n");

  size = 29393;
  new_start = (char *)(NEW_KERNEL_ADDR);//addr;
  new_end = new_start + size;

  old_start = (char*)&_start;
  old_end = (char*)&_end;
  uart_puts("Old Start: ");
  uart_puts(ftoa((int)old_start, buf, 5));
  uart_puts("\n");
  uart_puts("Old End: ");
  uart_puts(ftoa((int)old_end, buf, 5));
  uart_puts("\n");
  /*
  if (overlap(old_start, old_end, new_start, new_end) == 1) { //Overlap
    uart_puts("overlap!!\n");
    char *tmp_addr = (char *)(TMP_KERNEL_ADDR);
    while (old_start <= old_end) {
      *tmp_addr= *old_start;
      old_start++;
      tmp_addr++;
    }
  }

  */
  uart_puts("start loading!\n");
  // read the kernel
  for (int num = 0; num < size; num++) {
    *(new_start + num) = uart_getb();
    uart_puts(ftoa(num, buf, 5));
    uart_puts("\n");
  }
  uart_puts("ready to jump!\n");
  // restore arguments and jump to the new kernel.
  /*
  asm volatile (
    "mov x0, x10;"
    "mov x1, x11;"
    "mov x2, x12;"
    "mov x3, x13;"
    // we must force an absolute address to branch to
    "mov x30, 0x70000; ret"
  );
  */
  asm volatile("br %0" :: "r"((char *)(NEW_KERNEL_ADDR)));
}
