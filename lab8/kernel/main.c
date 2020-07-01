#include "bh.h"
#include "buddy.h"
#include "fat.h"
#include "framebuffer.h"
#include "irq.h"
#include "libc.h"
#include "miniuart.h"
#include "mm.h"
#include "sd.h"
#include "shell.h"
#include "slab.h"
#include "tmpfs.h"
#include "vfs.h"

void disable_aligned_check() {
  unsigned long long flag;
  __asm__ volatile("mrs %0, sctlr_el1" : "=r"(flag));
  flag &= ~(0b1010);
  flag |= 0b1 << 6;
  /* // flag |= 0b1 << 28; */
  __asm__ volatile("msr sctlr_el1, %0" ::"r"(flag));
  __asm__ volatile("mrs %0, sctlr_el1" : "=r"(flag));

}

int main(int argc, char *argv[]) {

  bh_mod_mask = 0;

  /* initialize uart with default clock & baud rate */
  uart_init();

  /* initialize framebuffer */
  lfb_init();

  /* create the buddy system */

#define GB_1_PAGES (128)
  memzero((unsigned long)LOW_MEMORY, GB_1_PAGES * 4096);
  struct buddy *bd = Buddy.new(GB_1_PAGES, LOW_MEMORY);
  uart_println("New a buddy");
  global_bd = bd;
  if (!bd) {
    uart_println("erorr while constructing the buddy system");
    return -1;
  }

  /* init slab allocator */
  kalloc_init(global_bd);

  /* vvvvvvvvvv filesystem part vvvvvvvvvvv */

  /* create a filesystem */
  /* struct filesystem *tmpfs = create_tmpfs(); */

  /* /\* create a mount point for filesystem above *\/ */
  /* { */
  /*   struct mount *m = kalloc(sizeof(struct mount)); */
  /*   rootfs = m; */
  /* } */

  /* tmpfs->setup_mount(tmpfs, rootfs); */

  /* initialize the sd card */
  sd_init();

  /* create a fat file system */
  struct filesystem *fat32 = create_fat32();
  {
    struct mount *m = kalloc(sizeof(struct mount));
    rootfs = m;
  }
  fat32->setup_mount(fat32, rootfs);

  shell();

  return 0;
}
