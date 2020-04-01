/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "uart.h"
#include "mbox.h"
#include "lfb.h"
#include "power.h"

char cmd [256];

void dbg_getline()
{
    int i,cmdidx=0,cmdlast=0;
    char c;
    cmd[0] = 0;
    // prompt
    printf("\r> ");
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
        printf("\r> %s \r\e[%dC",cmd,cmdidx+2);
    }
    printf("\n");
}

int strcmp(const char* s1, const char* s2)
{
    while (*s1 && (*s1 == *s2)){
        s1++;
	s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}


void main()
{
	// set up serial console and linear frame buffer
	uart_init();
	lfb_init();

	// display a pixmap
    	lfb_showpicture();
	
	//get board imformations 
	get_board_revision();
	get_vc_memory();

	printf("basic shell: \n");
	while(1){
		dbg_getline();
		if(!strcmp(cmd, "help")) {
		    printf( "help: print all available commands\n"
		            "hello: print Hello World!\n"
		            "reboot: reboot rpi3\n"
		            "timestamp: get current timestamp\n");
		}
		else if(!strcmp(cmd, "hello")) {
		    printf("Hello World!\n");
		}
		else if(!strcmp(cmd, "reboot")) {
		    printf("rebooting..\n");
		    reset();
		}
		else if(!strcmp(cmd, "timestamp")) {
		    unsigned long clock_freq;
		    unsigned long clock_cnt;
		    asm volatile("mrs %0, cntfrq_el0" : "=r" (clock_freq));
		    asm volatile("mrs %0, cntpct_el0" : "=r" (clock_cnt));
		    printf("[%d]\n", clock_cnt/clock_freq);
		}
		else {
		    printf("Error: Command %s not found, try <help>\n", cmd);
		}	
	}
}
