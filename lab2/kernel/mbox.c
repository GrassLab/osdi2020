#include "io.h"
#include "stdint.h"
#include "kernel.h"

void mbox_send(uint8_t channel, uint32_t *mailbox)
{
	uint32_t r = (((uint32_t)((unsigned long)mailbox) & ~0x0000000F) |
		      (channel & 0xF));
	while (*MBOX_STATUS & MBOX_FULL)
		;
	*MBOX_WRITE = r;
}

int mbox_read(uint8_t channel, uint32_t *mailbox)
{
	uint32_t r = ((uint32_t)((unsigned long)mailbox & ~0x0000000F) |
		      (channel & 0xF));
	do {
		while (*MBOX_STATUS & MBOX_EMPTY)
			;
	} while (r != *MBOX_READ);
	return mailbox[1] == MBOX_RESPONSE;
}