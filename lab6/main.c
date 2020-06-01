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

    /*
    int PFN;
    PFN = buddy_alloc(32);
    PFN =buddy_alloc(7);
    PFN = buddy_alloc(64);
    PFN = buddy_alloc(56);
    buddy_free(PFN);
    PFN = buddy_alloc(56); 
    */

    /*
    fix_allocator *alloc =  fix_allocator_init(64);
    char *test = (char*)fix_alloc(alloc);
    test = (char*)fix_alloc(alloc);
    test = (char*)fix_alloc(alloc);
    fix_free(alloc, (void*)test);
    test = (char*)fix_alloc(alloc);
	*/

    dynamic_allocator *dyalloc =  dynamic_allocator_init();
    char *test = dynamic_alloc(dyalloc, 64);
    test = dynamic_alloc(dyalloc, 128);
    dynamic_free(dyalloc, test);
    test = dynamic_alloc(dyalloc, 200);


    test[0] = '\0'; // make gcc happy :-)
}
