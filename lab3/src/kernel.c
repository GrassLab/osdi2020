#include "io.h"
#include "shell.h"
#include "uart.h"
#include "lfb.h"
#include "mbox.h"
#include "info.h"
#include "irq.h"

#ifdef TEST
#include "test.h"
#endif

#ifdef BUGGY
char *welcome = 
"              _                          "     NEWLINE
"__      _____| | ___ ___  _ __ ___   ___ "     NEWLINE
"\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" NEWLINE
" \\ V  V /  __/ | (_| (_) | | | | | |  __/"       NEWLINE
"  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|"  NEWLINE;
#endif

#if !defined (WITHOUT_LOADER) && !defined(NO_RELOC_SELF)
char *_kend, *_kbeg;
#endif

#define HCR_RW	    			(1 << 31)
#define HCR_VALUE			HCR_RW

void init_interrupt(){
	
    //__asm__ volatile ("ldr	x0, =0x80000000");
    //__asm__ volatile ("msr	hcr_el2, x0");
    unsigned long hcr, daif;
    __asm__ volatile ("mrs %0, hcr_el2" : "=r"(hcr));
    hcr |= 1 << 4;
    __asm__ volatile ("msr hcr_el2, %0" :: "r"(hcr));
	*ENABLE_IRQS_1 = (SYSTEM_TIMER_IRQ_1 | AUX_IRQ);
    __asm__ volatile ("msr  daifclr, #2 ");
    return;
	
    
    
    __asm__ volatile ("mrs %0, daif" : "=r"(daif));
    daif &= ~(0b1111 << 6);
    __asm__ volatile ("msr daif, %0" :: "r"(daif));

}

int main(void){

    uart_init();

    lfb_init();
    lfb_showpicture();

    init_interrupt();

#ifdef TEST
    puts("");
    ASSERT(bss_all_be() == 0);
    ASSERT((unsigned long long)stack_pointer() < (unsigned long long)0x80000);
#endif

    while(1){
#ifdef BUGGY
        println(NEWLINE, welcome);
#else
        println("              _                          "     );
        println("__      _____| | ___ ___  _ __ ___   ___ "     );
        println("\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\" );
        println(" \\ V  V /  __/ | (_| (_) | | | | | |  __/"       );
        println("  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|"  NEWLINE);
#endif

#ifdef BUILD_STAMP
#define xstr(a) str(a)
#define str(a) #a
        println(" BUILD @ ", xstr(BUILD_STAMP), NEWLINE);
#endif

#if !defined (WITHOUT_LOADER) && !defined(NO_RELOC_SELF)
        printf("   kernel segment: 0x%x - 0x%x" NEWLINE NEWLINE,
                &_kbeg, &_kend);
#endif

        println("UART TYPE: ", UART_TYPE);
        if(get_board_revision()) printf("Board revision: %x" NEWLINE, mbox[5]);
        if(get_arm_memaddr()) printf("ARM base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
        if(get_vc_memaddr()) printf("VC Core base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
        puts("");
        flush();
        shell_loop(); 
    }
    return 0;
}
