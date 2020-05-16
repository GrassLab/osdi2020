#include "./printf.h"
#include "./user_lib.h"

#define VA_START 			0xffff000000000000
#define DEVICE_BASE 		0x3F000000	
#define PBASE 			(VA_START + DEVICE_BASE)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)

// This function is required by printf function
void putc ( void* p, char c)
{
	call_sys_write((unsigned int)&c);
}

void init_print() {
    init_printf(0, putc);
}