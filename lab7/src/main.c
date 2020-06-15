#include <stddef.h>
#include <stdint.h>

#include "tools.h"

#include "mailbox.h"


void test_buddy(){
    uart_puts("=====TEST on Buddy System=====\n");
    buddy_init(PAGING_PAGES);
    buddy_show();

    // int alloc_num = 11;
    // int alloc_size[] = {34, 66, 35, 67, 1024, 4, 16, 17, 257, 53, 0};
    int alloc_num = 5;
    int alloc_size[] = {34, 66, 35, 67, 1024};
    int alloced_pool[alloc_num];
    for(int i=0;i<alloc_num;i++){
        alloced_pool[i] = buddy_alloc(alloc_size[i]);
        buddy_show();
    }

    for(int i=alloc_num-1;i>=0;i--){
        if(alloced_pool[i]==-1)continue;
        buddy_free(alloced_pool[i], alloc_size[i]);
        buddy_show();
    }

}
int alloc_num = 5;
int chunk_size1 = 1024;
int chunk_size2 = 512;
int chunk_size3 = 68;

void test_slab(){
    buddy_init(PAGING_PAGES);    
    init_all_allocator();
    // uart_puts("\n=====TEST on Fixed-sized allocator=====\n");
    int allocator_id = init_allocator(chunk_size1);
    int allocator_id1 = init_allocator(chunk_size2);
    int allocator_id2 = init_allocator(chunk_size3);
    int addrs[alloc_num];

    /*test continuously allocator*/
    for(int i=0;i<alloc_num;i++){
        uart_puts("\n--------------------------------\n");
        show_allocator(allocator_id);
        addrs[i] = alloc(allocator_id);
        

        uart_puts("\n");
        uart_puts("addr: ");
        uart_hex(addrs[i]);
        uart_puts("\n");
    }
    show_allocator(allocator_id);
    uart_puts("\n--------------------------------\n");
    for(int i=0;i<alloc_num;i++){
        uart_puts("\n--------------------------------\n");
        show_allocator(allocator_id);
        free_alloc(allocator_id, addrs[i]);  
        show_allocator(allocator_id);      
    }
    uart_puts("\n--------------------------------\n");

    /*test different size*/
    // uart_puts("\n--------------------------------\n");
    // addrs[0] = alloc(allocator_id);
    // uart_puts("\n");
    // uart_puts("addr: ");
    // uart_hex(addrs[0]);
    // uart_puts("\n");
    // show_allocator(allocator_id);
    // free_alloc(allocator_id, addrs[0]);
    // show_allocator(allocator_id);
    // uart_puts("\n--------------------------------\n");
    // addrs[1] = alloc(allocator_id1);
    //  uart_puts("\n");
    // uart_puts("addr: ");
    // uart_hex(addrs[1]);
    // uart_puts("\n");
    // show_allocator(allocator_id1);
    // free_alloc(allocator_id1, addrs[1]);
    // show_allocator(allocator_id1);
    // uart_puts("\n--------------------------------\n");
    // addrs[2] = alloc(allocator_id2);
    //  uart_puts("\n");
    // uart_puts("addr: ");
    // uart_hex(addrs[2]);
    // uart_puts("\n");
    // show_allocator(allocator_id2);
    // free_alloc(allocator_id2, addrs[2]);
    // show_allocator(allocator_id2);
    // uart_puts("\n--------------------------------\n");

    uart_puts("\n=====TEST on Varied-sized allocator=====\n");
    init_var_allocator();
    const int alloc_num = 6;
    int alloc_size[] = {34, 66, 35, 67, 1024, 4098};
    int alloced_pool[alloc_num];
    for(int i=0;i<alloc_num;i++){
        uart_puts("\n--------------------------------\n");
        alloced_pool[i] = var_alloc(alloc_size[i]);
        uart_puts("Get address: ");
        uart_hex(alloced_pool[i]);
        uart_puts("\n");
    }
    uart_puts("\n--------------------------------\n");

    for(int i=0;i<alloc_num;i++){
        uart_puts("\n--------------------------------\n");
        uart_puts("Free address: ");
        uart_hex(alloced_pool[i]);
        uart_puts("\n");
        var_free(alloced_pool[i], alloc_size[i]);
    }
    uart_puts("\n--------------------------------\n");

}
void main(){
    // set up serial console
    uart_init();
    //init_printf(NULL, putc);
    //mmu_init();

    // get_vc_information();
    // get_arm_information();
    // get_serial();
    // get_board_revision();
    // call_sys_enable_time();
  
    //init_init_task(shell);
    // extern unsigned long _binary_user_code_test_img_start;
    // unsigned long program_start = (unsigned long)&_binary_user_code_test_img_start;
    // echo everything back
    //privilege_task_create(program_start);
    //schedule();
    //print_hello();
    
    //uart_puts("# ");
    //shell();

    // uart_puts("LOW_MEMORY: ");
    // uart_hex(LOW_MEMORY);
    // uart_puts("\n");
    // uart_puts("HIGH_MEMORY: ");
    // uart_hex(HIGH_MEMORY);
    // uart_puts("\n");
    // uart_puts("PAGING_MEMORY: ");
    // uart_send_int(PAGING_MEMORY);
    // uart_puts("\n");
    // uart_puts("PAGING_PAGES: ");
    // uart_send_int(PAGING_PAGES);
    // uart_puts("\n");


    // buddy_init(PAGING_PAGES);

    uart_puts("R1 R2 test\n\n");

    filesystem fs = tmpfs_filesystem();
    register_filesystem(&fs);

    uart_puts("-----------------------\n");

    file* a = vfs_open("hello", O_OPEN);
    assert(a == NULL);

    uart_puts("-----------------------\n");

    a = vfs_open("hello", O_CREAT);
    assert(a != NULL);
    vfs_close(a);

    uart_puts("-----------------------\n");
    
    file* b = vfs_open("hello", O_OPEN);
    file* c = vfs_open("hello", O_OPEN);
    assert(b != NULL);
    vfs_close(b);
    vfs_close(c);

    uart_puts("=======================\n");

    uart_puts("R3 test\n\n");

    file* e = vfs_open("hello", O_OPEN);

    uart_puts("-----------------------\n");

    file* f = vfs_open("world", O_CREAT);

    uart_puts("-----------------------\n");
    vfs_write(e, "H", 1);
    uart_puts("-----------------------\n");
    vfs_write(e, "e", 1);
    uart_puts("-----------------------\n");
    vfs_write(e, "ll", 2);
    uart_puts("-----------------------\n");
    vfs_write(e, "o ", 2);
    uart_puts("-----------------------\n");

    vfs_write(f, "Wor", 3);
    uart_puts("-----------------------\n");
    vfs_write(f, "ld!", 3);

    uart_puts("=======================\n");
    vfs_close(e);
    vfs_close(f);
    uart_puts("-----------------------\n");
    f = vfs_open("hello", 0);
    uart_puts("-----------------------\n");
    e = vfs_open("world", 0);
    uart_puts("-----------------------\n");
    char buf[32];
    int sz;
    sz = vfs_read(f, buf, 100);
    uart_puts("-----------------------\n");
    sz += vfs_read(e, buf + sz, 100);
    uart_puts("-----------------------\n");
    uart_puts("buf: ");
    uart_puts(buf);
    uart_puts("\n");
        
    shell();
}	
