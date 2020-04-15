#include "exception.h"
#include "uart.h"
#define LOCAL_TIMER_IRQ_CLR		((volatile unsigned int*)(0x40000038))
#define CORE0_IRQ_SOURCE		((volatile unsigned int*)(0x40000060))
#define LOCAL_TIMER_CONTROL_REG		((volatile unsigned int*)(0x40000034))

extern void core_timer_enable();
extern void core_timer_handler();
int core_cnt=0,local_cnt=0,src;
int get_current_exception_level(int show){//show on screen if show==1, else keep silence
	unsigned int current_el=0;
	asm volatile("mrs %0, CurrentEL\n":"=r"(current_el):"r"(current_el):);
	current_el=current_el>>2;
	current_el&=0x3;
	if(show==0)
		return current_el;
	uart_puts("Current exception level: ",26);
	uart_send_int(current_el);
	uart_puts(" \n",0);
	return current_el;
}
void interrupt_enable(){
	unsigned long int hcr=0;
	unsigned int pstate=0;
	unsigned int flag = 0x30000000;
	unsigned int reload = 25000000;
	unsigned int ex_level=get_current_exception_level(0);
	if(ex_level==2){
		asm volatile("mrs %0, HCR_EL2\n":"=r"(hcr):"r"(hcr):);
		hcr|=1<<4;
		asm volatile("msr HCR_EL2, %0\n"::"r"(hcr):);
	}
	core_timer_enable();
	*LOCAL_TIMER_CONTROL_REG=flag|reload;
	asm volatile("msr DAIFClr, 0xf\n":::);
	uart_puts("irq done\n",0);
}
void exception_handler0(){uart_puts("except_0\n",0);}
void exception_handler1(){uart_puts("except_1\n",0);}
void exception_handler2(){uart_puts("except_2\n",0);}
void exception_handler3(){uart_puts("except_3\n",0);}
void exception_handler4(){
	unsigned long int addr=0,esr=0,class=0,iss=0,current_el=0;
	uart_puts("except_4\n",0);
	current_el=get_current_exception_level(1);
	if(current_el==1){
		asm volatile("mrs %0, ELR_EL1\n":"=r"(addr):"r"(addr):);
	}
	else if(current_el==2){
		asm volatile("mrs %0, ELR_EL2\n":"=r"(addr):"r"(addr):);
	}
	uart_puts("Exception return address 0x",27);
	uart_hex(addr);
	if(current_el==1){
		asm volatile("mrs %0, ESR_EL1\n":"=r"(esr):"r"(esr):);
	}
	else if(current_el==2){
		asm volatile("mrs %0, ESR_EL2\n":"=r"(esr):"r"(esr):);
	}
	iss=esr&0x1ffffff;
	class=esr>>26;
	uart_puts("Exception class (EC) 0x",24);
	uart_hex(class);
	uart_puts("Instruction specific syndrome (ISS) 0x",38);
	uart_hex(iss);

	return;
}
void exception_handler5(){
	asm volatile("msr DAIFSet, 0xf\n":::);
	src=*CORE0_IRQ_SOURCE;
	int current_el=get_current_exception_level(1);
	uart_puts("except_5\n",0);
	if(src==2){
		core_timer_handler();
		++core_cnt;
		uart_puts("Core timer interrupt, ",22);
		uart_puts("jiffies: ",9);
		uart_send_int(core_cnt);
	}
	else if(src==2048){
		*LOCAL_TIMER_IRQ_CLR = 0xc0000000;
		++local_cnt;
		uart_puts("Local timer interrupt, ",23);
		uart_puts("jiffies: ",9);
		uart_send_int(local_cnt);
	}
	uart_puts(" \n",0);
	asm volatile("msr DAIFClr, 0xf\n":::);
	return;
}
void exception_handler6(){uart_puts("except_6\n",0);}
void exception_handler7(){uart_puts("except_7\n",0);}

void exception_handler8(){
	unsigned long int addr=0,esr=0,class=0,iss=0,current_el=0;
	uart_puts("except_8\n",0);
	current_el=get_current_exception_level(1);
	asm volatile("mrs %0, ELR_EL1\n":"=r"(addr):"r"(addr):);
	uart_puts("Exception return address 0x",27);
	uart_hex(addr);
	asm volatile("mrs %0, ESR_EL1\n":"=r"(esr):"r"(esr):);
	iss=esr&0x1ffffff;
	class=esr>>26;
	uart_puts("Exception class (EC) 0x",24);
	uart_hex(class);
	uart_puts("Instruction specific syndrome (ISS) 0x",38);
	uart_hex(iss);
	if(class==21 && iss==5){
		interrupt_enable();
	}
	asm volatile("bl from_el1_to_el0\n":::);
	//asm volatile("eret\n":::);
}
void exception_handler9(){
	asm volatile("msr DAIFSet, 0xf\n":::);
	//asm volatile("bl save_all\n":::);
	src=*CORE0_IRQ_SOURCE;
	int current_el=get_current_exception_level(1);
	uart_puts("except_9\n",0);
	if(src==2){
		core_timer_handler();
		++core_cnt;
		uart_puts("Core timer interrupt, ",22);
		uart_puts("jiffies: ",9);
		uart_send_int(core_cnt);
	}
	else if(src==2048){
		*LOCAL_TIMER_IRQ_CLR = 0xc0000000;
		++local_cnt;
		uart_puts("Local timer interrupt, ",23);
		uart_puts("jiffies: ",9);
		uart_send_int(local_cnt);
	}
	uart_puts(" \n",0);
	//asm volatile("bl load_all\n":::);
	asm volatile("bl from_el1_to_el0\n":::);
	asm volatile("msr DAIFClr, 0xf\n":::);

}
void exception_handlerA(){uart_puts("except_A\n",0);}
void exception_handlerB(){uart_puts("except_B\n",0);}

void exception_handlerC(){uart_puts("except_C\n",0);}
void exception_handlerD(){uart_puts("except_D\n",0);}
void exception_handlerE(){uart_puts("except_E\n",0);}
void exception_handlerF(){uart_puts("except_F\n",0);}
