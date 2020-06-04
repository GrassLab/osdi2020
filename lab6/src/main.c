#include "gpio.h"
#include "uart.h"
#include "printf.h"
#include "sched.h"
#include "shell.h"
#include "timer.h"
#include "mm.h"
#include "buddy.h"
#define S_MAX 1000

void foo(){
    while(1) {
        // printf("Task id: %d\n", current -> taskid);
        delay(1000000);
        schedule();
    }
}

void idle(){
    while(1){
        schedule();
        delay(1000000);
    }
}

void el1_main(){
    // set up serial console
    uart_init();

    core_timer_enable();

    int N = 3;
    for(int i = 0; i < N; ++i) { // N should > 2
        privilege_task_create(foo, i);
    }

    idle();
    
}

void main(){
    // set up serial console
    uart_init();

    // say hello
    uart_puts("Hello World!\n");
    
    // echo everything back
    while(1) {
        char str[S_MAX];

        uart_puts("# ");
        read_string(str);

        
        if(strcmp(str,"")==0){
            ;
        }else if(strcmp(str,"buddy")==0){
            printf("LOW memory: %x\n", LOW_MEMORY);
            printf("HIGH memory: %x\n", HIGH_MEMORY);
            printf("PAGING_MEMORY: %x\n", PAGING_MEMORY);
            printf("PAGING_PAGES: %x\n", PAGING_PAGES);

            #define GB_1_PAGES ((1 << 30) >> 12)

            buddy_init(3000);
            buddy_show();

            int alloc_num = 3;
            int alloc_size[] = {7, 530, 1030};
            
            int alloced_pool[alloc_num];
            for(int i=0;i<alloc_num;i++){
                alloced_pool[i] = buddy_alloc(alloc_size[i]);
                buddy_show();
            }

            for(int i=0;i<alloc_num;i++){
                if(alloced_pool[i]==-1) continue;
                buddy_free(alloced_pool[i], alloc_size[i]);
                buddy_show();
            }

            
        }else{
            printf("Err: command %s not found, try <help>\n", str);
        }
    }

    return;
}
