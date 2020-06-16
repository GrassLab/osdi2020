#include "bh.h"
#include "buddy.h"
#include "framebuffer.h"
#include "irq.h"
#include "libc.h"
#include "miniuart.h"
#include "mm.h"
#include "shell.h"
#include "slab.h"
#include "tmpfs.h"
#include "vfs.h"

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
  struct filesystem *tmpfs = create_tmpfs();

  /* create a mount point for filesystem above */
  {
    struct mount *m = kalloc(sizeof(struct mount));
    rootfs = m;
  }

  tmpfs->setup_mount(tmpfs, rootfs);

  /* create a file */
#define assert(exp)                                                            \
  {                                                                            \
    if (!(exp))                                                                \
      return -1;                                                               \
  }

  uint8_t buf[512] = {
      0,
  };

  /* struct path *p = path_name_solver("../../"); */
  /* /\* struct path *r = path_name_solver("x/hello/world"); *\/ */

  /* uart_println("=======path list======"); */
  /* int flag = 0; */
  /* while (p && p->name) { */
  /*   if (flag++) { */
  /*     uart_print("->"); */
  /*   } */
  /*   uart_print("%s", p->name); */
  /*   p = p->next; */
  /* } */
  /* uart_println(""); */
  /* uart_println("====================="); */

  /* test1 */
  vfs_mkdir("/mnt");
  struct file *a = vfs_open("/mnt/hello.txt", O_CREAT);

  uart_println("[[ list file under \"/mnt/\" ]]");
  uart_println("=============================");

  struct file *root = vfs_open("/mnt", 0);

  uart_println("=============================");

  vfs_write(a, "Hello ", 6);

  vfs_chdir("mnt");
  struct file *b = vfs_open("./world.txt", O_CREAT);
  vfs_write(b, "World!", 6);

  vfs_close(a);
  vfs_close(b);

  vfs_chdir("..");

  b = vfs_open("mnt/hello.txt", 0);
  a = vfs_open("mnt/world.txt", 0);

  int sz;
  sz = vfs_read(b, buf, 100);
  sz += vfs_read(a, buf + sz, 1);
  sz += vfs_read(a, buf + sz, 2);
  sz += vfs_read(a, buf + sz, 3);
  sz += vfs_read(a, buf + sz, 4);
  buf[sz] = '\0';
  uart_println("%s\n", buf); // should be Hello World!


  vfs_open("/mnt", 0);

  uart_println("============= set mount ============");
  vfs_mount("tmpfs", "mnt", "tmpfs");
  uart_println("====================================");

  vfs_open("/mnt", 0);

  uart_println("============= umount ============");
  vfs_umount("mnt");
  uart_println("=================================");

  vfs_open("/mnt", 0);

  /* /\* create a directory *\/ */
  /* vfs_mkdir("dir"); */
  /* uart_println("[[ list file under \"root\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* vfs_chdir("dir"); */
  /* struct file *c = vfs_open("another_hello", O_CREAT); */
  /* struct file *d = vfs_open("another_world", O_CREAT); */
  /* uart_println("[[ list file under \"dir\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* vfs_close(c); */
  /* vfs_close(d); */

  /* vfs_chdir(".."); */
  /* uart_println("[[ list file under \"dir's ..\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* vfs_mkdir("dir/new-dir"); */
  /* vfs_chdir("dir"); */
  /* uart_println("[[ list file under \"dir\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* vfs_chdir("new-dir/"); */
  /* struct file *e = vfs_open("file-under-new-dir", O_CREAT); */
  /* vfs_write(e, "This is a file under new-dir", 28); */
  /* vfs_close(e); */
  /* uart_println("[[ list file under \"new-dir\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* vfs_chdir("../.."); */

  /* uart_println("[[ list file under \"../..\" ]]"); */
  /* uart_println("============================="); */
  /* vfs_open(".", 0); */
  /* uart_println("============================="); */

  /* uart_println("=========== open dir/new-dir/file-... ============"); */
  /* struct file *f = vfs_open("dir/new-dir/file-under-new-dir", 0); */

  /* uint8_t another_buf[512] = { */
  /*     0, */
  /* }; */

  /* sz = vfs_read(f, another_buf, 100); */
  /* another_buf[sz] = '\0'; */
  /* uart_println("%s", another_buf); // should be This is a file under new-dir
   */

  /* uart_println("=================================================="); */

  /* test 1 */
  /* struct file *a = vfs_open("hello", 0); */
  /* assert(a == NULL); */

  /* a = vfs_open("hello", O_CREAT); */
  /* assert(a != NULL); */

  /* vfs_close(a); */

  /* struct file *b = vfs_open("hello", 0); */
  /* assert(b != NULL); */

  /* vfs_close(b); */

  /* struct file *b = vfs_open("hello", 0); */
  /* assert(b != NULL); */
  /* vfs_close(b); */

  /* the end of the bss segment */

  /* struct filesystem tmpfs; */

  /* construct a file system */
  /* struct filesystem tmpfs; */

  /* uart_println("Tmpfs from : %x to %x", , LOW_MEMORY); */

  /* start interactive shell */
  /* while (1); */
  shell();

  return 0;
}
