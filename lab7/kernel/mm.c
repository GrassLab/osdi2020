#include "stdint.h"
#include "kernel.h"
#include "mm.h"
#include "uart.h"

struct page page_map[TOTAL_PAGE_NUM] = { FREE };

void create_pgd()
{
	page_alloc();
}
void page_table_init()
{
	extern uint64_t _kernel_start, _kernel_end; //	already align
	uint32_t kernel_start_ppn = ((uint64_t)&_kernel_start) >> 12;
	uint32_t kernel_end_ppn = ((uint64_t)&_kernel_end) >> 12;
	// uart_hex(kernel_start_ppn);
	// uart_hex(kernel_end_ppn);
	for (; kernel_start_ppn < kernel_end_ppn; kernel_start_ppn++) {
		page_map[kernel_start_ppn].used = USED;
		// uart_hex(kernel_start_ppn * 4096);
		// uart_puts("\n");
	}
}

extern uint64_t page_dir;
void *virt_to_phy(void *virt)
{
	uint64_t pfn = ((uint64_t)virt & ~0xFFFF000000000000) >> 12;
	uint32_t offset = (uint32_t)virt & 0xFFF;
	uart_hex(*(uint64_t *)((pfn * 4096 + offset) + page_dir));
	return (void *)(pfn * 4096 + offset);
}

uint64_t phy_to_ppn(void *phy)
{
	return (uint64_t)phy >> 12;
}

/* NOTICE AVOID SEARCH IN KERNEL MEMORY */
uint64_t *page_alloc()
{
	uint64_t free_ppn = 0;
	while (free_ppn < TOTAL_PAGE_NUM) {
		if (page_map[free_ppn].used == FREE) {
			page_map[free_ppn].used = USED;
			return (uint64_t *)(free_ppn << 12);
		}
		free_ppn++;
	}
	return (uint64_t *)0;
}

int page_free(void *addr)
{
	print("%x\n", (uint64_t)virt_to_phy(addr));
	uint64_t ppn = phy_to_ppn(virt_to_phy(addr));
	if (page_map[ppn].used == FREE)
		return -1;
	page_map[ppn].used = FREE;
	print("Freed 0x");
	uart_hex(addr);
	print("\n");
	return 0;
}

void page_descriptor(uint64_t pfn, void *phy_addr)
{
}
