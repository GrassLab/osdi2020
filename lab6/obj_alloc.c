#include "obj_alloc.h"
#include "mm.h"
#include "string.h"
#include "uart.h"

int fix_obj_alloc(int size) {
  for (int i = 0; i < fix_object_count; i++) {
    if (fix_object_array[i].is_use == 0) {
      return i;
    }
  }
  if (fix_object_count == 100) {
    printf("full\n");
    return -1;
  } else {
    unsigned long addr = page_alloc(1 * page_size);
    if (addr == -1) {
      printf("alloc error\n");
      return -1;
    }

    unsigned long create_size = page_size / size;
    printf("page_size = %d\n", page_size);
    printf("create_size = %d\n", create_size);
  }
  return 1;
}