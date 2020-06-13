#include "uart.h"
#include "mystd.h"
#include "buddy.h"
#include "allocator.h"

// #include "exc.h"
// #include "irq.h"
//#include "task.h"
// #include "timer.h"

void main()
{
    uart_init();
    buddy_init();

    int PFN;
    char *addr;

    uart_puts("================================== buddy system ==================================\n");
    PFN = buddy_alloc(32);
    PFN =buddy_alloc(7);
    PFN = buddy_alloc(64);
    PFN = buddy_alloc(56);
    buddy_free(PFN);
    PFN = buddy_alloc(56); 
    

    uart_puts("\n\n================================== fix allocator ==================================\n");
    buddy_init();
    fix_allocator *alloc =  fix_allocator_init(64);
    addr = (char*)fix_alloc(alloc);
    addr = (char*)fix_alloc(alloc);
    addr = (char*)fix_alloc(alloc);
    fix_free(alloc, (void*)addr);
    addr = (char*)fix_alloc(alloc);
	
    uart_puts("\n\n================================== dynamic allocator ==================================\n");
    buddy_init();
    dynamic_allocator *dyalloc =  dynamic_allocator_init();
    addr = dynamic_alloc(dyalloc, 64);
    addr = dynamic_alloc(dyalloc, 128);
    dynamic_free(dyalloc, addr);
    addr = dynamic_alloc(dyalloc, 200);
    addr = dynamic_alloc(dyalloc, 5000);
    dynamic_free(dyalloc, addr);


    addr[0] = '\0'; // make gcc happy :-)
}
