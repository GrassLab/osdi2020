#include "uart.h"
#include "exception.h"
#include "my_shell.h"

extern void from_el1_to_el0();
extern void from_el2_to_el1();

void print_figlet();
void main(){

	unsigned long int ex_table=0,current_el=0;
	uart_init();
	//cancel_reset();
	//uart_puts("HELLO WORLD!\n",0);
	print_figlet();
	current_el=get_current_exception_level(1);
	asm volatile("mrs %0, VBAR_EL2\n":"=r"(ex_table):"r"(ex_table):);
	uart_puts("exception table addr: ",22);
	uart_hex(ex_table);
	//asm volatile("bl save_all\n":::);
	//asm volatile("bl from_el2_to_el1\n":::);
	//asm volatile("bl load_all\n":::);
	current_el=get_current_exception_level(1);
	//asm volatile("bl from_el2_to_el1\n":::);
	//current_el=get_current_exception_level(1);
	//from_el1_to_el0();
	shell();
}

void print_figlet(){
	uart_puts(":'#######:::'######::'########::'####:'##::::::::::'###::::'########:::'#######::\n",0);
	uart_puts("'##.... ##:'##... ##: ##.... ##:. ##:: ##:::::::::'## ##::: ##.... ##:'##.... ##:\n",0);
	uart_puts(" ##:::: ##: ##:::..:: ##:::: ##:: ##:: ##::::::::'##:. ##:: ##:::: ##:..::::: ##:\n",0);
	uart_puts(" ##:::: ##:. ######:: ##:::: ##:: ##:: ##:::::::'##:::. ##: ########:::'#######::\n",0);
	uart_puts(" ##:::: ##::..... ##: ##:::: ##:: ##:: ##::::::: #########: ##.... ##::...... ##:\n",0);
	uart_puts(" ##:::: ##:'##::: ##: ##:::: ##:: ##:: ##::::::: ##.... ##: ##:::: ##:'##:::: ##:\n",0);
	uart_puts(". #######::. ######:: ########::'####: ########: ##:::: ##: ########::. #######::\n",0);
	uart_puts(":.......::::......:::........:::....::........::..:::::..::........::::.......:::\n",0);
}

