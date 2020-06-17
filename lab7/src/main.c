#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#include "timer.h"
#include "kernel.h"
#include "mm.h"
#include "buddy.h"
#include "vfs.h"
#define S_MAX 1000

void main(){
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");

    buddy_init(100);
    buddy_show();

    __init_kmalloc();
    
    // echo everything back
    while(1) {
        char str[S_MAX];

        uart_puts("# ");
        read_string(str);

        if(strcmp(str,"")==0){
            ;
        }else if(strcmp(str,"r2")==0){
            rootfs_init();
            struct file* a = vfs_open("hello", 0);
            printf("%d\n", a);

            a = vfs_open("hello", O_CREAT);
            printf("%d\n", a->vnode);
            vfs_close(a);

            struct file * b = vfs_open("hello", 0);
            printf("%d\n", b->vnode);
            vfs_close(b);
        }else if(strcmp(str,"r3")==0){
            rootfs_init();

            struct file* a = vfs_open("hello", O_CREAT);
            struct file* b = vfs_open("world", O_CREAT);
            vfs_write(a, "Hello ", 6);
            vfs_write(b, "World!", 6);
            vfs_close(a);
            vfs_close(b);
            b = vfs_open("hello", 0);
            a = vfs_open("world", 0);
            char buf[100];
            int sz;
            sz = vfs_read(b, buf, 100);
            sz += vfs_read(a, buf + sz, 100);
            buf[sz] = '\0';
            printf("\n%s\n", buf); // should be Hello World!

            list_tmpfs(rootfs->dentry);
        }else{
            printf("Err: command %s not found, try <help>\n", str);
        }
    }

    return;
}
