#include "uart.h"
// #include "../fs/vfs.h"
// #include "../fs/tmpfs.h"
#include "../allocator/buddy.h"
#include "../allocator/obj_allocator.h"

extern unsigned long _end;

void uart_test()
{
    // set up serial console
    uart_init();
    
    // test our printf implementation
    printf("hex: 0x%x\n", &_end);
    char aaa[0x10];
    strcpy(aaa, "aaa");
    printf("aaa == %s\n", aaa);
    printf("%d\n", strcmp(aaa, "b"));
    printf("%d\n", strncmp(aaa, "a", 1));

    // echo everything back
    while(1) {
        uart_send(uart_getc());
    }
}

#define NULL 0
#define assert(expr) if(!(expr)){printf("assert error!!!\n");} \

// void testcase1()
// {
//     log("+++++ testcase1 +++++\n");
//     struct file* a = vfs_open("hello", 0);
//     assert(a == NULL);
//     a = vfs_open("hello", O_CREAT);
//     assert(a != NULL);
//     vfs_close(a);
//     struct file* b = vfs_open("hello", 0);
//     assert(b != NULL);
//     vfs_close(b);
// }

// void testcase2()
// {
//     log("+++++ testcase2 +++++\n");
//     char buf[0x1000];
//     struct file* a = vfs_open("hello", O_CREAT);
//     struct file* b = vfs_open("world", O_CREAT);
//     vfs_write(a, "Hello ", 6);
//     vfs_write(b, "World!", 6);
//     vfs_close(a);
//     vfs_close(b);
//     b = vfs_open("hello", 0);
//     a = vfs_open("world", 0);
//     int sz;
//     sz = vfs_read(b, buf, 100);
//     sz += vfs_read(a, buf + sz, 100);
//     buf[sz] = '\0';
//     printf("%s\n", buf); // should be Hello World!
// }

// void testcase3()
// {
//     log("+++++ testcase3 +++++\n");
//     // create some regular files at root directory
//     struct file* root = vfs_open("/", 0);
//     struct file* a = vfs_open("hello", O_CREAT);
//     struct file* b = vfs_open("world", O_CREAT);
//     // your read directory function
//     // iterate all directory entries and print each file's name.
//     vfs_ls(root);
// }

void testcase4()
{
    buddy_view();
    unsigned long addr1 = buddy_alloc(0x100);
    unsigned long addr2 = buddy_alloc(0x100);
    // buddy_view();
    printf("addr1: 0x%lx\n", addr1);
    printf("addr2: 0x%lx\n", addr2);
    buddy_view();
    
    buddy_free(addr1);
    buddy_free(addr2);
    buddy_view();
}

void testcase5()
{
    unsigned long token1 = registration(0x100);
    unsigned long addr1 = fixed_alloc(token1);
    unsigned long addr2 = fixed_alloc(token1);
    unsigned long addr3 = fixed_alloc(token1);
    // printf("token = 0x%lx\n", token);
    allocator_view();
    // printf("addr1 = 0x%lx\n", addr1);
    fixed_free(addr1);
    fixed_free(addr2);
    fixed_free(addr3);
    allocator_view();
}

void testcase6()
{
    printf("sizeof(struct file): %d\n", 0x28);
    unsigned long addr1 = malloc(0x28);
    // unsigned long addr2 = malloc(0x4000);
    printf("addr1 = 0x%lx\n", addr1);
    // printf("addr2 = 0x%lx\n", addr2);
    free(addr1);
    // free(addr2);
    // buddy_view();
    // allocator_view();

}


void main()
{
    // set up serial console
    uart_init();
    buddy_init();
    init_allocator();
    
    // test our printf implementation
    printf("Hello %s!\n", "hank0438");
    // printf("hex: 0x%x\n", &_end);

    printf("This is demo of Varied-size object allocator\n");
    testcase6();

    // log("This is demo of VFS!\n");
    // setup_tmpfs_filesystem();
    // testcase3();
}
