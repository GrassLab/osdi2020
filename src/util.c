#include "../include/util.h"
#include "../include/uart.h"
void Print_Int(unsigned long long int input)
{
	int Flag = 0, i;
	char digit;
	char ar[100];
	//init array
	for(i=0;i<100;i++) ar[i] = '0';
	//put input into array
	i = 99;
	while(input > 0 && i > 0)
	{
		digit = (input % 10) + '0';
		input = input / 10;
		ar[i--] = digit;
	}
	//print
	for(i=0;i<100;i++)
	{
		if(Flag == 0)
		{
			if(ar[i] == '0') continue;
			else
			{
				Flag = 1;
				uart_send(ar[i]);
			}
		}
		else uart_send(ar[i]);
	}
}

int StrCmp(char *input, char* command, int input_length, int command_length)
{
	int i, Equal = 1;
	if(input_length != command_length) return 0;
	for(i=0;i<input_length;i++)
	{
		if(input[i] != command[i]) Equal = 0;
	}
	return Equal;
}