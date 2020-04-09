#include "../include/uart.h"
/**
 * helper to read a line from user. We redefine some control caracters to handle CSI
 * \e[3~ = 1, delete
 * \e[D  = 2, cursor left
 * \e[C  = 3, cursor right
 */
void read_cmdline(char *cmd)
{
    int i,cmdidx=0,cmdlast=0;
    char c;
    cmd[0]=0;
    // prompt
    uart_puts("\r> ");
    // read until Enter pressed
    while((c=uart_getc())!='\n') {
        // decode CSI key sequences (some, not all)
        if(c==27) {
            c=uart_getc();
            if(c=='[') {
                c=uart_getc();
                if(c=='C') c=3; else    // left
                if(c=='D') c=2; else    // right
                if(c=='3') {
                    c=uart_getc();
                    if(c=='~') c=1;     // delete
                }
            }
        }
        // Backspace
        if(c==8 || c==127) {
            if(cmdidx>0) {
                cmdidx--;
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        } else
        // Delete
        if(c==1) {
            if(cmdidx<cmdlast) {
                for(i=cmdidx;i<cmdlast;i++) cmd[i]=cmd[i+1];
                cmdlast--;
            }
        } else
        // cursor left
        if(c==2) {
            if(cmdidx>0) cmdidx--;
        } else
        // cursor right
        if(c==3) {
            if(cmdidx<cmdlast) cmdidx++;
        } else {
            // is there a valid character and space to store it?
            if(c<' ' || cmdlast>=sizeof(cmd)-1) {
                continue;
            }
            // if we're not appending, move bytes after cursor
            if(cmdidx<cmdlast) {
                for(i=cmdlast;i>cmdidx;i--)
                    cmd[i]=cmd[i-1];
            }
            cmdlast++;
            cmd[cmdidx++]=c;
        }
        cmd[cmdlast]=0;
        // display prompt and command line, place cursor with CSI code
        
        char cmdline[256];
        cmdline[0] = '\e';
        cmdline[1] = '[';
        int cmdidx_size;
        cmdidx_size = uart_atoi(&cmdline[2], cmdidx+2);
        cmdline[cmdidx_size+2] = 'C';
        uart_puts("\r> ");
        uart_puts(cmd);
        uart_puts(" \r");
        uart_puts(cmdline);
    }
    uart_puts("\n");
}