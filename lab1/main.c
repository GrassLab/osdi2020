#include "uart.h"
#include "str.h"

#define PM_PASSWORD 0x5a000000
#define PM_RSTC ((volatile unsigned int*)0x3F10001c)
#define PM_WDOG ((volatile unsigned int*)0x3F100024)

void reset(int tick){ // reboot after watchdog timer expire
	*PM_RSTC = (PM_PASSWORD | 0x20);
	*PM_WDOG = (PM_PASSWORD | tick);
	/*set(PM_RSTC, PM_PASSWORD | 0x20); // full reset
	set(PM_WDOG, PM_PASSWORD | tick); // number of watchdog tick*/
}

void cancel_reset() {
	*PM_RSTC = (PM_PASSWORD | 0);
	*PM_WDOG = (PM_PASSWORD | 0);
	/*set(PM_RSTC, PM_PASSWORD | 0); // full reset
	set(PM_WDOG, PM_PASSWORD | 0); // number of watchdog tick*/
}

// 1B 5B 41~44 上下右左
void myread(char *str, int max_size)
{
	int count=0;
	char input_c, *now=str, *tmpc;
	while(1){
		if(count > max_size)
			break;
		input_c = uart_getc();
		if(input_c == '\n' || input_c=='\0')
		{
			uart_puts("\r\n");
			break;
		}
		else if(input_c == 0x1B)
		{
			input_c = uart_getc();
			if(input_c == 0x5B)
			{
				input_c = uart_getc();
				if(input_c == 0x43)
				{
					if(*(now+1)!='\0' && *now!='\0'){
						uart_puts("\x1B\x5B\x43");
						now++;
					}
				}
				if(input_c == 0x44)
				{
					if(*now != *str){
						uart_puts("\x1B\x5B\x44");
						now--;
					}
				}
			}
		}
		else if(input_c == 0x7f && count > 0) //backspace
		{
			if(*now != 0)
			{
				uart_send(input_c);
				tmpc=now-1;
				while(*tmpc!=0)
				{
					*tmpc=*(tmpc+1);
					uart_send(*tmpc);
					tmpc++;
				}
				uart_send(' ');
				while(*tmpc--!=*now)
					uart_puts("\x1B\x5B\x44");
				uart_puts("\x1B\x5B\x44");
				if(*(now+1) == '\0')
					uart_puts("\x1B\x5B\x44");
				--now;
			}
			else if(count > 0){
				*(--now) = '\0';
				count--;
				uart_send(input_c);
			}
			continue;
		}
		else{
			uart_send(input_c);
			if(*now != 0)
			{
				tmpc=now;
				while(*tmpc){
					uart_send(*tmpc);
					tmpc++;
				}
				while(*tmpc!=*now) {
					*tmpc=*(tmpc-1);
					uart_puts("\x1B\x5B\x44");
					tmpc--;
				}
			}
			*now = input_c;
			count++;
			now++;
		}
	}
	*(str+count) = '\0';
}

void mywrite(char *str)
{
	uart_puts(str);
}

void cal_time(volatile unsigned int time_FRQ, volatile unsigned int time_CT, char *char_time)
{
	/*float timer = (float)time_CT/time_FRQ;
	int int_part = (int)timer;
	timer = timer - (float)int_part;
	int point_part = (int)(timer*1000000);
	my_itoa(int_part, char_time, 10);
	char point[10];
	my_itoa(point_part, point, 10);
	my_strcat(char_time, ".");
	my_strcat(char_time, point);*/
	int tmp,count=0;
	my_itoa((int)(time_CT / time_FRQ), char_time, 10);
	my_strcat(char_time, ".");

	tmp = (time_CT % time_FRQ)*10;
	char *p = char_time + my_strlen(char_time);
	while(tmp>0 && count < 10){
		*p++ = (char)((tmp / time_FRQ)+48);
		tmp = (tmp % time_FRQ)*10;
		count++;
	}
	*p = '\0';
}
void main()
{
	volatile unsigned int time_FRQ;
	asm volatile("mrs %0, CNTFRQ_EL0" : "=r"(time_FRQ) ::);
	/*asm volatile("mrs x1, CNTFRQ_EL0");
	asm volatile("str x1, [sp, #96]");*/
	volatile unsigned int time_CT;
    // set up serial console
    uart_init();
	mywrite("Hello~~ try 'help' \n");
	char *command[5] = {"help", "hello", "timestamp", "reboot", 0};
	char *command_detial[5] = {" : help", " : print Hello World!", " : get current timestamp", " : reboot system", 0};
    char input[100]={0};
	char len[100]={0};
	char timer[100] = {0};
    while(1){
		my_strset(input, 0, 100);
		my_strset(len, 0, 100);
		mywrite(">> ");
		my_strset(len, 0, 100);
		myread(input, 100);
		/*
		my_itoa(my_strlen(input), len, 10); //test for the input size
		mywrite(len);
		*/
		if(my_strcmp(input, command[0], my_strlen(input)) == 0)
        {
            for(char **i = command,**j=command_detial; *i; i++, j++)
            {
                mywrite(*i);
				mywrite(*j);
                mywrite("\n");
            }
        }
        else if(my_strcmp(input, command[1], my_strlen(input)) == 0)
        {
            mywrite("Hello World!\n");
        }
		else if(my_strcmp(input, command[2], my_strlen(input)) == 0)
		{
			asm volatile("mrs %0, cntpct_el0" : "=r"(time_CT)::);
			my_strset(timer, 0, 100);
			cal_time(time_FRQ, time_CT, timer);
			mywrite(timer);
			mywrite("\n");
		}
		else if(my_strcmp(input, command[3], my_strlen(input)) == 0)
		{
			mywrite("reboot time:");
			my_strset(input, 0, 100);
			myread(input, 100);
			reset(my_atoi(input));
		}
		else
		{
			mywrite("Err command ");
			mywrite(input);
			mywrite(" not found, try <help>");
			/*my_strset(len, 0, 100);
			my_itoa((int)(*input), len, 10);
			mywrite(len);*/
			mywrite("\n");
		}
	}
	
}