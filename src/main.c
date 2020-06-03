#include "shell.h"

void main() {
  // set up serial console
  uart_init();
  uart_flush();
  char *osdi = "OSDI Welcome\n";
  uart_puts(osdi);

  /*
  struct page* p0, *p1;
  p0 = page_alloc();
  p1 = page_alloc();
  page_free(p0);
  p0 = page_alloc();
  */

  init_buddy_system();
  struct block* m1 = get_space(3 << 12);
  struct block* m2 = get_space(5 << 12);
  struct block* m3 = get_space(5 << 12);
  struct block* m4 = get_space(5 << 12);
  struct block* m5 = get_space(5 << 12);

  free_space(m1);
  free_space(m5);

  int token = register_obj_allocator(4, FIXED);
  void* obj[7];
  for(int i = 0; i < 7; i++){
    obj[i] = fixed_obj_allocate(token);
    uart_puts("memory allocate at: ");
    uart_hex(obj[i]);
    uart_puts("\n");
  }
  for(int i = 0; i < 7; i++){
    fixed_obj_free(obj[i], token);
  }

  uart_puts("\n\n varied size allocate\n");

  int token_1 = register_obj_allocator(3, VARIED);
  int token_2 = register_obj_allocator(127, VARIED);
  void* obj_1[7], *obj_2[7];
  /*
  for(int i = 0; i < 7; i++){
    obj_1[i] = varied_obj_allocate(token_1);
    uart_puts("memory allocate at: ");
    uart_hex(obj_1[i]);
    uart_puts("\n");
  }
  for(int i = 0; i < 7; i++){
    varied_obj_free(obj_1[i], token_1);
  }
  */

  for(int i = 0; i < 7; i++){
    obj_2[i] = varied_obj_allocate(token_2);
    uart_puts("memory allocate at: ");
    uart_hex(obj_2[i]);
    uart_puts("\n");
  }
  for(int i = 0; i < 7; i++){
    varied_obj_free(obj_2[i], token_2);
  }


  for(;;) {
    put_shell();
  }
}
