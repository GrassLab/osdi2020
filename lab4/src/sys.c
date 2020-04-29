#include "tools.h"
#include "mm.h"
void sys_write(char * buf){
	uart_puts(buf);
}
int sys_read(){
	uart_getc();
}

int sys_clone(unsigned long stack){
	// return copy_process(0, 0, 0, stack);
    return 0;
}

unsigned long sys_malloc(){
	unsigned long addr = get_free_page();
	if (!addr) {
		return -1;
	}
	return addr;
}

void sys_exit(){
    exit();
}

void sys_chk_exl_info(){
    int address, syndrome;
    asm volatile ("mrs %0, elr_el1\n"
            "mrs %1, esr_el1\n":"=r" (address), "=r" (syndrome));
    int iss = syndrome & 0x01ffffff;
    int ec = (syndrome & 0xfc000000) >> 26;
	uart_puts("Exception return address: ");
    uart_hex(address);
    uart_send('\n');
    uart_puts("Exception class (EC): ");
    uart_hex(ec);
    uart_send('\n');    
    uart_puts("Instruction specific syndrome (ISS): ");
    uart_hex(iss);
    uart_send('\n');
}
void sys_chk_exl(){
    uart_puts("Exception level: ");               
    int el = get_el();
    uart_send_int(el);
    uart_puts("\n");
}
void sys_enable_time(){
	core_timer_enable();
}

int sys_get_taskid(){
	return get_taskid();
}


void * const sys_call_table[] = {sys_write, sys_malloc, sys_clone, sys_exit, sys_chk_exl, sys_enable_time, sys_chk_exl_info, sys_get_taskid, sys_read};
