# Lab 5 : Virtual memory ## Requirement 1 


### Translation Control Register (TCR)
 - [ ] `required 1-1`  Set up TCR_EL1.
 - [ ] `required 1-2`  Set up MAIR_EL1.
 - [ ] `required 1-3`  Set up identity mapping.
 - [ ] `required 1-4`  Modify linker script and map the upper address space.

 - [ ] `question 1` Without indirect branch, the code might still work fine, why it’s the case and why it’s mandatory to use indirect branch.

 一開始執行是kernel code 所以可以看到實體位址
 但是一旦回到user mode 就會認不出來在哪裡 所以要用indirect branch

 - [ ] `required 1-5` Linear map kernel with finer granularity and map RAM as normal memory.

 - [ ] `question 2` For mapping 1GB memory region, how many page frames are used by page tables(PGD, PUD, PMD, and PTE) in four level translation?

```
page size 2^12 =  4096
1*1*1*256*4096 = 1048576 = 1GB
(1個PGD+1個PUD+1個PMD+256個PTE)
```