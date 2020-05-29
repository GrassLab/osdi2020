#include "uart.h"
#include "mystd.h"
#include "buddy.h"

// #include "exc.h"
// #include "irq.h"
//#include "task.h"
// #include "timer.h"

void main()
{
    uart_init();
    buddy_init();

    int PFN;

    PFN = buddy_alloc(32);
    PFN = buddy_alloc(7);
    PFN = buddy_alloc(64);
    PFN = buddy_alloc(56);
    buddy_free(PFN);
    PFN = buddy_alloc(56);


}
