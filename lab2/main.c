#include "uart.h"
#include "mbox.h"
#include "mystd.h"
#include "lfb.h"

#define CMDSIZE 64
char cmd[CMDSIZE] = {0};
int cmdSize = 0;

void get_board_ver(){
    set_board_ver_mbox();

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My board version is: ");
        uart_hex(mbox[5]);
        uart_puts("\n");
    } else {
        uart_puts("Unable to query baord version!\n");
    }
}

void get_VC_Mem_base_size(){
    set_VC_Mem_base_size_mbox();

    if (mbox_call(MBOX_CH_PROP)) {
        uart_puts("My VC Memory Base is: 0x");
        uart_hex(mbox[5]);
        uart_puts("\nMy VC Memory size is: 0x");
        uart_hex(mbox[6]);
        uart_puts(" bytes \n");
    } else {
        uart_puts("Unable to query VC Memroy Base!\n");
    }
}

void queryUART(){
    set_queryUART0_mbox();
    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("[UART0]");
        unsigned int mask = 3;
        unsigned int res = mbox[6] & mask;

        if(res == 0) uart_puts("device exist, power state: OFF\n");
        else if(res == 1) uart_puts("device exist, power state: ON\n");
        else if(res == 2) uart_puts("device does not exist\n");
    }else{
        uart_puts("Unable to query UART0 power state!\n");   
    }

    set_queryUART1_mbox();
    if(mbox_call(MBOX_CH_PROP)){
        uart_puts("[UART1]");
        unsigned int mask = 3;
        unsigned int res = mbox[6] & mask;

        if(res == 0) uart_puts("device exist, power state: OFF\n");
        else if(res == 1) uart_puts("device exist, power state: ON\n");
        else if(res == 2) uart_puts("device does not exist\n");
    }else{
        uart_puts("Unable to query UART1 power state!\n");   
    }

    return;
}


void loadimg(){
    int size=0;
    char *kernel=(char*)0x80000;

again:
    uart_send('\r');
    uart_send('\n');
    // notify raspbootcom to send the kernel
    uart_send(3);
    uart_send(3);
    uart_send(3);

    // read the kernel's size
    size=uart_getc();
    size|=uart_getc()<<8;
    size|=uart_getc()<<16;
    size|=uart_getc()<<24;

    // send negative or positive acknowledge
    if(size<64 || size>1024*1024) {
        // size error
        uart_send('S');
        uart_send('E');
        goto again;
    }
    uart_send('O');
    uart_send('K');

    // read the kernel
    while(size--) *kernel++ = uart_getc();

    // restore arguments and jump to the new kernel.
    asm volatile (
        "mov x0, x10;"
        "mov x1, x11;"
        "mov x2, x12;"
        "mov x3, x13;"
        // we must force an absolute address to branch to
        "mov x30, 0x80000; ret"
    );

}

void cmd_process(){
    uart_puts("\r");

    if(strcmp(cmd, "loadimg")){
        loadimg();
    }else if(strcmp(cmd, "uartstate")){
        queryUART();
    }else if(strcmp(cmd, "uart1")){
        PL011_uart_init();
        uart_puts("\n");
    }else if(strcmp(cmd, "help")){
        uart_puts("command: \"help\" Description: \"print all available commands\"  \n");
        uart_puts("command: \"loadimg\" Description: \"loading image by UART\"  \n");
    }
    else if(strlen(cmd) != 0){
        uart_puts("command \"");
        uart_puts(cmd);
        uart_puts("\" not found, try <help> \n");   
    }

    uart_puts("# ");
}

void cmd_push(char c){
    if(cmdSize<CMDSIZE) {
        switch(c){
            case 8:     //backspace
                if(cmdSize>0) cmdSize--;
                uart_send(32);
                uart_send(8);
                break;
            case 10:    // 0X0A '\n' newline,  0X0D '\r' carriage return
                cmd[cmdSize] = '\0';
                cmdSize++;
                cmd_process();
                cmdSize = 0;
                break;

            default :
                cmd[cmdSize] = c;
                cmdSize++;  
        }
    }
}

void main()
{
    // set up serial console
    mini_uart_init();

    lfb_init();
    lfb_showpicture();
    
    uart_puts("\rstart up.... \n");
    get_board_ver();
    get_VC_Mem_base_size();

    uart_puts("# ");

    while(1){
        char c = uart_getc();

        if(c < 127){
            uart_send(c);       //show character user typed on uart terminal
            // printASCII(c);
            cmd_push(c);
        }

    }
}
