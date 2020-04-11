#include "utility.h"
#include "gpio.h"

#define PM_PASSWORD   0x5a000000
#define PM_RSTC       ((volatile unsigned int*)(0x3F10001c))
#define PM_RSTS       ((volatile unsigned int*)(0x3F100020))
#define PM_WDOG       ((volatile unsigned int*)(0x3F100024))

void printPowerOnMessage() {
    uart_puts("    ...        *                        *       *\n \
      ...   *         * ..   ...                        *\n \
 *      ...        *           *            *\n \
          ...               ...                          *\n \
            ..                            *\n \
    *        ..        *                       *\n \
           __##____              *                      *\n \
  *    *  /  ##  ****                   *\n \
         /        ****               *         *  X   *\n \
   *    /        ******     *                    XXX      *\n \
       /___________*****          *             XXXXX\n \
        |            ***               *       XXXXXXX   X\n \
    *   | ___        |                    *   XXXXXXXX  XXX\n \
  *     | | |   ___  | *       *             XXXXXXXXXXXXXXX\n \
        | |_|   | |  ****             *           X   XXXXXXX\n \
    *********** | | *******      *                X      X\n \
************************************************************\n");
}

void printDebugPowerOnMessage()
{
    uart_puts("               /|___\n \
             ///|   ))\n \
           /////|   )))\n \
         ///////|    )))\n \
       /////////|     )))\n \
     ///////////|     ))))\n \
   /////////////|     )))\n \
  //////////////|    )))\n \
////////////////|___)))\n \
  ______________|________\n \
  \                    /\n \
~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void setRegister(unsigned int address, unsigned int value)
{
    *(volatile unsigned int*)(address) = value;
}

unsigned int getRegister(unsigned int address) 
{
    return *(volatile unsigned int*)(address);
}

void stackSave() 
{
    asm volatile("stp     x0, x1, [sp, #-16]!");
    asm volatile("stp     x2, x3, [sp, #-16]!");
    asm volatile("stp     x4, x5, [sp, #-16]!");
    asm volatile("stp     x6, x7, [sp, #-16]!");
    asm volatile("stp     x8, x9, [sp, #-16]!");
    asm volatile("stp     x10, x11, [sp, #-16]!");
    asm volatile("stp     x12, x13, [sp, #-16]!");
    asm volatile("stp     x14, x15, [sp, #-16]!");
    asm volatile("stp     x16, x17, [sp, #-16]!");
    asm volatile("stp     x18, x19, [sp, #-16]!");
    asm volatile("stp     x20, x21, [sp, #-16]!");
    asm volatile("stp     x22, x23, [sp, #-16]!");
    asm volatile("stp     x24, x25, [sp, #-16]!");
    asm volatile("stp     x26, x27, [sp, #-16]!");
    asm volatile("stp     x28, x29, [sp, #-16]!");
    asm volatile("str     x30,      [sp, #-8]!");
}

void stackRestore()
{
    asm volatile("ldr x30,      [sp, #8]");
    asm volatile("ldp x28, x29, [sp, #16]");
    asm volatile("ldp x26, x27, [sp, #16]");
    asm volatile("ldp x24, x25, [sp, #16]");
    asm volatile("ldp x22, x23, [sp, #16]");
    asm volatile("ldp x20, x21, [sp, #16]");
    asm volatile("ldp x18, x19, [sp, #16]");
    asm volatile("ldp x16, x17, [sp, #16]");
    asm volatile("ldp x14, x15, [sp, #16]");
    asm volatile("ldp x12, x13, [sp, #16]");
    asm volatile("ldp x10, x11, [sp, #16]");
    asm volatile("ldp x8, x9, [sp, #16]");
    asm volatile("ldp x6, x7, [sp, #16]");
    asm volatile("ldp x4, x5, [sp, #16]");
    asm volatile("ldp x2, x3, [sp, #16]");
    asm volatile("ldp x0, x1, [sp, #16]");
}

void reboot(int tick)
{
    unsigned int r = *PM_RSTS;
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_PASSWORD | r;
    *PM_WDOG = PM_PASSWORD | 30;
    *PM_RSTC = PM_PASSWORD | 0x20;
}

double getTimestamp()
{
    long long int cntfrq;
    long long int cntptc;
    asm volatile ("mrs %0, cntfrq_el0" : "=r" (cntfrq));
    asm volatile ("mrs %0, cntpct_el0" : "=r" (cntptc));
    return ((double)cntptc / cntfrq);
}

void memset(void* mem, int value, int size) {
    unsigned char *ptr = (unsigned char*) mem;
    while (size-- > 0)
        *ptr++ = value;
}

int pow(int num, int p)
{
    int n = num;
    if (p == 0) return 1;
    for(int i = 0; i < p-1; i++) {
        num = num * n;
    }
    return num;
}