#include "uart.h"
#include "string.h"
#include "utility.h"
#include "mailbox.h"
#include "printf.h"
#include "memory.h"
#include "allocator.h"
#include "fat32.h"
#include "sdlib.h"
#include "vfs.h"

#define NULL 0

void init_rootfs() {
    fat32fs = (FileSystem *) malloc(sizeof(FileSystem));
    fat32fs->name = "fat32fs";
    fat32fs->setup_mount = fat32_setup_mount;
    register_filesystem(fat32fs);
}

void test_lab8_req1() {
    vfs_ls(rootfs->root);
    char buf[1024] = {0};
    File *file = vfs_open("START.ELF", 0);
    vfs_read(file, buf, 540);
    for (int i = 0 ; i < 540; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void test_lab8_req2() {
    File *file = vfs_open("START.ELF", 0);
    char *str = "Hello World";
    char buf[1024] = {0};
    vfs_write(file, str, 11);
    vfs_read(file, buf, 100);
    for (int i = 0 ; i < 100; i++) {
        printf("%c", buf[i]);
    }
    printf("\n");
}

void test_lab8_req3() {
    File *file = vfs_open("START.ELF", 0);
    char str[600] = {0};
    char buf[1024] = {0};
    for (int i = 0; i < 600; i++) {
        str[i] = 'C';
    }
    vfs_write(file, str, 600);
    vfs_read(file, buf, 700);
    for (int i = 0 ; i < 700; i++) {
        printf("%c", buf[i]);
    }
}

int main()
{
    uart_init();
    init_printf(0, putc);
    page_sys_init();
    allocator_init();
    varied_size_init();
    init_rootfs();
    test_lab8_req1();
    test_lab8_req2();
    test_lab8_req3();
}
