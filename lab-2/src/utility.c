#include "utility.h"
#include "gpio.h"

#define PM_PASSWORD 0x5a000000
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