#include <stdint.h>

#include "io.h"
#include "lfb.h"
#include "mbox.h"
#include "reset.h"
#include "shell.h"
#include "uart.h"

#define GET_BOARD_REVISION 0x00010002
#define REQUEST_CODE 0x00000000
#define REQUEST_SUCCEED 0x80000000
#define REQUEST_FAILED 0x80000001
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

#define GET_VC_MEMORY 0x00010006
#define UART_ID 0x000000002

extern void core_timer_enable(int a);
char cmd[200];

void get_board_revision() {
    mbox[0] = 7 * 4;  // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_BOARD_REVISION;  // tag identifier
    mbox[3] = 4;  // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;  // value buffer
    // tags end
    mbox[6] = END_TAG;

    mbox_call(
        MBOX_CH_PROP);  // message passing procedure call, you should implement
                        // it following the 6 steps provided above.

    print_s("Board revision: 0x");
    print_h(mbox[5]);
    print_s("\n");
}

void get_vc_base() {
    mbox[0] = 8 * 4;  // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    // tags begin
    mbox[2] = GET_VC_MEMORY;  // tag identifier
    mbox[3] = 8;  // maximum of request and response value buffer's length.
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0;  // value buffer
    mbox[6] = 0;  // value buffer
    // tags end
    mbox[7] = END_TAG;

    mbox_call(
        MBOX_CH_PROP);  // message passing procedure call, you should implement
                        // it following the 6 steps provided above.

    print_s("VC core base address: 0x");
    print_h(mbox[5]);
    print_s("\n");
    print_s("VC core base size: 0x");
    print_h(mbox[6]);
    print_s("\n");
}

int strcmp(const char *p1, const char *p2) {
    const unsigned char *s1 = (const unsigned char *)p1;
    const unsigned char *s2 = (const unsigned char *)p2;
    unsigned char c1, c2;
    do {
        c1 = (unsigned char)*s1++;
        c2 = (unsigned char)*s2++;
        if (c1 == '\0') return c1 - c2;
    } while (c1 == c2);
    return c1 - c2;
}

void read_cmd() {
    char now;
    int now_cur = 0;
    while ((now = read_c()) != '\n') {
        if (now == 127) {  // delete
            now_cur -= 1;
            if (now_cur >= 0) {
                print_s("\b \b");
                now_cur -= 1;
            }
        } else {
            cmd[now_cur] = now;
            print_c(now);
        }
        now_cur++;
    }
    print_s("\n");
    cmd[now_cur] = 0;
}

void shell() {
    print_s("# ");
    read_cmd();
    if (!strcmp(cmd, "help")) {
        print_s(
            "help      : print this help menu\n"
            "hello     : print Hello World!\n"
            "timestamp : print system timestamp\n"
            "clear     : clear screen\n"
            "reboot    : reboot the device\r\n");
    } else if (!strcmp(cmd, "hello")) {
        print_s("Hello World!\r\n");
    } else if (!strcmp(cmd, "timestamp")) {
        uint64_t frq, ct;
        asm volatile("mrs %0, cntpct_el0" : "=r"(ct));
        asm volatile("mrs %0, cntfrq_el0" : "=r"(frq));
        double frq_f = frq, ct_f = ct;
        print_i(ct_f / frq_f);
        print_s("\r\n");
    } else if (!strcmp(cmd, "reboot")) {
        reset(10);
    } else if (!strcmp(cmd, "clear")) {
        print_s("\033[2J\033[1;1H");
    } else if (!strcmp(cmd, "hw")) {
        get_board_revision();
        get_vc_base();
    } else if (!strcmp(cmd, "exc")) {
        asm volatile("svc #1");
    } else if (!strcmp(cmd, "irq")) {
        asm volatile("svc #2");
        /* int wait = read_i(); */
        /* core_timer_enable(0xffffff); */
    } else {
        /* print_s("command not found: "); */
        /* print_s(cmd); */
        /* print_s("\r\n"); */
    }
}
