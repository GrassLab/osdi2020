# Lab4 Questions

> Q: Without indirect branch, the code might still work fine, why it’s the case and why it’s mandatory to use indirect branch.
A: ttbr0_el1 and ttbr1_el1 points to the same page frame. All map to 0x00000000 to 0x3fffffff, which is the same as physical address.  We need to switch to virtual address otherwise otherwise ldr won't work well. Because PC is still in physical address space.

> Q: For mapping 1GB memory region, how many page frames are used by page tables(PGD, PUD, PMD, and PTE) in four level translation?
A: GPD: 1, PUD: 1, PMD: 512(262144/512), PTE: 262144(1GB/4KB)

> Q: If a page frame is allocated and to be mapped at user space. Is it necessary to initialize it into 0?
A: Depends on the use of the user mode program. If it's for .bss segment then yes. Others may not be necessary.

