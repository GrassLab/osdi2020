#include "info.h"
#include "io.h"
#include "irq.h"
#include "lfb.h"
#include "mbox.h"
#include "shell.h"
#include "timer.h"
#include "sched.h"
#include "uart.h"
#include "mm.h"

#ifdef TEST
#include "test.h"
#endif

#ifdef RUN_ON_EL0
#define EL 0
#else
#define EL 1
#endif

#if !defined(WITHOUT_LOADER) && !defined(NO_RELOC_SELF)
char *_kend, *_kbeg;
#endif

#include "mm.h"

int main(void) {

  uart_init();
  //lfb_init();
  //lfb_showpicture();

#ifdef TEST
  puts("");
  ASSERT(bss_all_be() == 0);
  ASSERT((unsigned long long)stack_pointer() < (unsigned long long)0x80000);
#endif

  puts("HELLO WORLD SUCCESSFULLY");

  while (1) {

    println("              _                          ");
    println("__      _____| | ___ ___  _ __ ___   ___ ");
    println("\\ \\ /\\ / / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\");
    println(" \\ V  V /  __/ | (_| (_) | | | | | |  __/");
    println("  \\_/\\_/ \\___|_|\\___\\___/|_| |_| |_|\\___|" NEWLINE);

#ifdef BUILD_STAMP
#define xstr(a) str(a)
#define str(a) #a
    println(" BUILD @ ", xstr(BUILD_STAMP), NEWLINE);
#endif

#if !defined(WITHOUT_LOADER) && !defined(NO_RELOC_SELF)
    printf("   kernel segment: 0x%x - 0x%x" NEWLINE NEWLINE, &_kbeg, &_kend);
#endif
    mark_reserved_pages((unsigned long)&_kend);

    println("UART TYPE: ", UART_TYPE);

  //  if (get_board_revision())
  //    printf("Board revision: %x" NEWLINE, mbox[5]);
  //  if (get_arm_memaddr())
  //    printf("ARM base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
  //  if (get_vc_memaddr())
  //    printf("VC Core base addr: 0x%x size 0x%x" NEWLINE, mbox[5], mbox[6]);
  //  puts("");
  //  flush();


    core_timer_init();

    privilege_task_create(kernel_process, 0, 1);

    while(1){
      puts("kernel main scheduling...");
      schedule();
    }

#ifdef UARTINT
    irq_shell_loop(EL);
#else
    busy_shell_loop(EL);
#endif
  }
  return 0;
}

void test(){
  puts("test hello world");
}
