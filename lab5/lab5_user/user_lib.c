#include "./printf.h"
#include "./user_lib.h"

#define VA_START 			0xffff000000000000
#define DEVICE_BASE 		0x3F000000	
#define PBASE 			(VA_START + DEVICE_BASE)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)

int strcmp(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return *str1 - *str2;
        }

        if (*str1 == '\0' && *str2 != '\0') {
            return 1;
        }
		else if (*str1 != '\0' && *str2 == '\0') {
			return 1;
		}
		else if (*str1 == '\0' && *str2 == '\0') {
			return 0;
		}
        str1++;
        str2++;
    }
    return 1;
}

// This function is required by printf function
void putc ( void* p, char c)
{
	write((unsigned long)&c);
}

void init_print() {
    init_printf(0, putc);
}