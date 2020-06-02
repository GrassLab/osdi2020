#include "obj_alloc.h"
#include "mm.h"
#include "string.h"
#include "uart.h"

int fix_obj_alloc() {
  int size = 2000;
  for (int i = 0; i < fix_object_count; i++) {
    if (fix_object_array[i].is_use == 0) {
      char *s = (char *)fix_object_array[i].obj;
      for (int j = 0; j < size; j++) {
        *(s + j) = 0;
      }
      fix_object_array[i].is_use = 1;
      return i;
    }
  }
  if (fix_object_count == 100) {
    printf("full\n");
    return -1;
  }
  unsigned long addr = page_alloc(1 * page_size);
  if (addr == -1) {
    printf("alloc error\n");
    return -1;
  }

  unsigned long create_size = page_size / size;
  printf("create_obj = %d\n", create_size);
  for (int i = fix_object_count; i < fix_object_count + create_size; i++) {
    fix_object_array[i].is_use = 0;
    fix_object_array[i].obj = (struct page_struct *)(addr + i * size);
  }
  int get = fix_object_count;
  fix_object_array[fix_object_count].is_use = 1;
  fix_object_count += create_size;
  return get;
}

int fix_obj_free(int free_index) {
  if (fix_object_array[free_index].is_use == 1) {
    fix_object_array[free_index].is_use = 0;
    return 1;
  }
  printf("free error ???????\n");
  return -1;
}