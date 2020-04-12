#include "printf.h"

void show_invalid_entry_message(unsigned long esr, unsigned long address)
{
	printf("ESR: %x, address: %x\r\n", esr, address);
}