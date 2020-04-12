#include "simple_shell.h"

#include "kernel_recv.h"
#include "mm.h"
#include "printf.h"
#include "string.h"
#include "uart.h"


#define PM_RSTC      ((volatile unsigned int*)(0x3F10001C))
#define PM_WDOG      ((volatile unsigned int*)(0x3F100024))
#define PM_RSTC_WRCFG_FULL_RESET  0x00000020
#define PM_PASSWORD  0x5A000000


char *promptStr="simple shell> ";

char *commandStr[NUM_COMMAND]=
{
	"help", "hello", "timestamp", "reboot", "loadimg"//, "exit"
};

char *commandDesc[NUM_COMMAND]={
	"Show commands available.", "Show \"Hello World!\"", "Get current timestamp.", "Reboot device.", "Load kernel by UART."
};

void (*commandArray[NUM_COMMAND])()=
{
	cmd_help, cmd_hello, cmd_timestamp, cmd_reboot, cmd_loadimg
};

/* 
* An infinity loop keep reading, parse, execute command
*/
void run_shell()
{
	char buff[MAX_COMMAND_LENGTH];
	int n, cmd_type;

	uart_puts(promptStr);
	while ((n=read_command(buff, MAX_COMMAND_LENGTH)) >= 0){
		buff[n]='\0';
		cmd_type = parse_command(buff);
		if (cmd_type==-1)
			printf("command '%s' not found, try <help>\n", buff);
		else
			commandArray[cmd_type]();
    	printf("\n%s", promptStr);
	}
}

int read_command(char *buff, unsigned int size)
{
	int count = 0;
	char char_recv;
	while (count < size){
		char_recv = uart_getc();
		uart_putc(0, char_recv);
		if (char_recv == '\n'){
			return count;
		} else if (char_recv == '\177'){// backspace
			if (count >= 1){
				uart_puts("\b \b");
				count --;
			}
		} else {
			buff[count] = char_recv;
			count ++;
		}
	}
	return -1; // out of buff	
}

// TODO:  parameter parse
int parse_command(char *buff)
{
	int i;
	for (i=NUM_COMMAND-1; i>=0; i--){
		if (strcmp(buff, commandStr[i]) == 0)
			break;
	}
	return i;
}

// TODO: formate string
void cmd_help()
{
	for(int i=0; i< NUM_COMMAND; i++){
		printf("%s: %s\n",commandStr[i], commandDesc[i]);
	}
}

void cmd_hello()
{
    uart_puts("Hello world!\n");
}

// TODO: hard-coding num_digit
void cmd_timestamp()
{
	int time, time_count, time_freq;
	asm volatile("mrs %0, cntpct_el0": "=r"(time_count)::); // read counts of core timer
	asm volatile("mrs %0, cntfrq_el0": "=r"(time_freq)::); // read frequency of core timer
	time = time_count / (time_freq / 100000);
	printf("[ %d.%d ]\n",time/100000, time%100000);
}

void _reboot(int tick){ // reboot after watchdog timer expire
	*PM_RSTC = PM_PASSWORD | PM_RSTC_WRCFG_FULL_RESET;// full reset
	*PM_WDOG = PM_PASSWORD | tick;// number of watchdog tick
}

void _cancel_reboot() {
	*PM_RSTC = PM_PASSWORD | 0; // full reset
	*PM_WDOG = PM_PASSWORD | 0; // number of watchdog tick
}

void cmd_reboot()
{
    uart_puts("rebooting ...");
	_reboot(50);// timeout = 1/16th of a second? (whatever)
	while(1);
}

void cmd_loadimg() {
	unsigned int n, size;
	void *load_addr;
	char buff[MAX_COMMAND_LENGTH];
	uart_puts("Input kernel size(bytes): ");
	if ((n = read_command(buff, MAX_COMMAND_LENGTH)) >= 0) {
		buff[n]='\0';
		size = atoi(buff);
		uart_puts("Input kernel load address: ");
		if ((n = read_command(buff, MAX_COMMAND_LENGTH)) >= 0) {
			buff[n]='\0';
			load_addr = (void*)(hex2int(buff));
		}
		uart_puts("Please send kernel image by UART!\n\n");
		// copy kernel_reciver
		void (*reciver_start)(unsigned int size, void *load_addr) = kernel_recv;
		memncpy((char *)reciver_start, load_addr-60, 60);
		// start load kernel
		reciver_start = load_addr-60;
		reciver_start(size, load_addr);
	}
}