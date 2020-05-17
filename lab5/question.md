# Lab4 Questions

> Q: Without indirect branch, the code might still work fine, why it’s the case and why it’s mandatory to use indirect branch.
A: ttbr0_el1 and ttbr1_el1 points to the same page frame. All map to 0x00000000 to 0x3fffffff, which is the same as physical address.  We need to switch to virtual address otherwise otherwise ldr won't work well. Because PC is still in physical address space.
