#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "printf.h"
#include "memory.h"
#include "allocator.h"
#include "tmpfs.h"
#include "vfs.h"

#define NULL 0

void init_rootfs() {
    tmpfs = (FileSystem *) malloc(sizeof(FileSystem));
    tmpfs->name = "tmpfs";
    tmpfs->setup_mount = tmpsfs_setup_mount;
    register_filesystem(tmpfs);
}

void test_lab7_req1() {
    File *a = vfs_open("hello_file", 0);
    if (a != NULL) printf("[This is an Error]\n");
    a = vfs_open("hello_file", O_CREAT);
    if (a == NULL) printf("[This is an Error]\n");;
    vfs_close(a);
    File *b = vfs_open("hello_file", 0);
    if (b == NULL) printf("[This is an Error]\n");;
    vfs_close(b);
    vfs_ls(rootfs->root);
}

void test_lab7_req2() {
    File* a = vfs_open("hello", O_CREAT);
    File* b = vfs_open("world", O_CREAT);
    vfs_write(a, "Hello ", 6);
    vfs_write(b, "World!", 6);
    vfs_close(a);
    vfs_close(b);
    b = vfs_open("hello", 0);
    a = vfs_open("world", 0);
    int sz;
    char buf[256];
    sz = vfs_read(b, buf, 100);
    sz += vfs_read(a, buf + sz, 100);
    buf[sz] = '\0';
    printf("%s\n", buf); // should be Hello World!
    vfs_mkdir(rootfs->root, "myfolder");
    vfs_ls(rootfs->root);
}

int main()
{
    uart_init();
    init_printf(0, putc);
    page_sys_init();
    allocator_init();
    varied_size_init();
    init_rootfs();
    printf("\n--- require 1 start ---\n\n");
    test_lab7_req1();
    printf("\n--- require 2 start ---\n\n");
    test_lab7_req2();
}
