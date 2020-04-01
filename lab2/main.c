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

void cmd_process(){
    uart_puts("\r");

    if(strcmp(cmd, "uartstate")){
        queryUART();
    }else if(strcmp(cmd, "uart1")){
        PL011_uart_init();
        uart_puts("\n");
    }else if(strcmp(cmd, "help")){
        uart_puts("command: \"help\" Description: \"print all available commands\"  \n");
        uart_puts("command: \"uart1\" Description: \"change uart0 to uart1\"  \n");
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
