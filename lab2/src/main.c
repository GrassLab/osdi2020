#include "mbox.h"
#include "uart.h"
#define GET_BOARD_REVISION 0x00010002
#define GET_VC_MEMORY 0x00010006
#define REQUEST_CODE 0x00000000
#define TAG_REQUEST_CODE 0x00000000
#define END_TAG 0x00000000

extern char _start_begin[];
extern char _end[];

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }
        if (*str1 == '\0') {
            return 0;
        }
        str1++;
        str2++;
    }
}

void memset(char *str, char c, int n) {
    for (int i = 0; i < n; i++) {
        str[i] = c;
    }
}

void get_board_revision() {
    mbox[0] = 7 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    /* tags begin */
    mbox[2] = GET_BOARD_REVISION; // tag identifier
    mbox[3] =
        4; // maximum of request and response value buffer's length in byte
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = END_TAG;

    uart_puts("Board revision: ");
    // message passing procedure call
    if (mbox_call(MBOX_CH_PROP) == 1) {
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("mailbox_call Fail! \n");
    }
    return;
}

void get_VC_Core_base_address() {
    mbox[0] = 8 * 4; // buffer size in bytes
    mbox[1] = REQUEST_CODE;
    /* tags begin */
    mbox[2] = GET_VC_MEMORY; // tag identifier
    mbox[3] =
        8; // maximum of request and response value buffer's length in byte
    mbox[4] = TAG_REQUEST_CODE;
    mbox[5] = 0; // value buffer
    mbox[6] = 0; // value buffer
    mbox[7] = END_TAG;

    uart_puts("VC Core ");
    // message passing procedure call
    if (mbox_call(MBOX_CH_PROP) == 1) {
        uart_puts("Base Address: ");
        uart_hex(mbox[5]);
        uart_puts(", Size: ");
        uart_hex(mbox[6]);
        uart_puts("\n");
    } else {
        uart_puts("mailbox_call Fail! \n");
    }
    return;
}

void recv_loadimg(char *address_loadto) {
    // read kernel size
    int size = 0;
    while (1) {
        char c = uart_getc();
        if (c == '\n') {
            break;
        }
        size *= 10;
        size += c - '0';
    }
    uart_puts("RecvSizeDone");

    // read the kernel
    char *kernel = address_loadto;
/*    int chunck = 256;
    for (int i = 0; i < size/chunck ; i++){
        for (int j = 0; j < chunck; j++) {
            char c = uart_getc();
            kernel[j] = c;
        }
        uart_puts("RecvChunckImgDone");
    }
    for (int j = 0; j < size%chunck; j++){
        char c = uart_getc();
        kernel[j] = c;
    }
*/
    for (int j = 0; j < size; j++) {
        char c = uart_getc();
        kernel[j] = c;
    }
    uart_puts("RecvImgDone");
    branch_to_address((unsigned long int *)address_loadto);
}

void loadimg() {
    // move original kernel
    char *ini = _start_begin;
    char *ini_end = _end;
    char *copy = (char *)0x100000;
    while (ini <= ini_end) {
        *copy = *ini;
        copy++;
        ini++;
    }
    uart_puts("PleaseLoadimg");
    // calculte new funcion address of recv_loadimg 
    char *address_loadto = (char *)0x90000;
    void (*func_ptr)() = recv_loadimg;
    unsigned long int func_address = (unsigned long int)func_ptr;
    void (*call_function)(char *) = (void (*)(char *))(
        func_address - (unsigned long int)_start_begin + 0x100000);
    call_function(address_loadto);
}

void main() {
    uart_init();
    uart_getc();
    uart_puts("MACHINE IS OPEN!!\n");
    uart_puts("# ");
    char str[200];
    int i = 0;
    memset(str, '\0', 200);

    while (1) {
        char c = uart_getc();
        str[i] = c;
        //uart_send(c);
        if (c == '\n') {
            if (strcmp(str, "revision\n") == 0) {
//                uart_puts("[entryRevision]");
                get_board_revision();
            } else if (strcmp(str, "loadimg\n") == 0) {
//                uart_puts("[entryLoadimg]");
                loadimg();
            } else if (strcmp(str, "address\n") == 0) {
//                uart_puts("[entryAddress]");
                get_VC_Core_base_address();
            } else {
//                uart_puts("[entryErr]");
                uart_puts("Err: ");
                str[i] = '\0';
                uart_puts(str);
                uart_puts(" command not found, try <help>\n");
            }
            i = 0;
            memset(str, '\0', 200);
            uart_puts("# ");
        } else {
            i++;
        }
    }
    return;
}
