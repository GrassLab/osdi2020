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

void reboot(int tick)
{
    unsigned int r = *PM_RSTS;
    r &= ~0xfffffaaa;
    *PM_RSTS = PM_PASSWORD | r;
    *PM_WDOG = PM_PASSWORD | 30;
    *PM_RSTC = PM_PASSWORD | 0x20;
}

void memset(void* mem, int value, int size) {
    unsigned char *ptr = (unsigned char*) mem;
    while (size-- > 0)
        *ptr++ = value;
}

void memcpy(void *src, void *dst, int size) {
    char *s = src;
    char *d = dst;
    while (size--) {
        *d-- = *s--;
    }
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