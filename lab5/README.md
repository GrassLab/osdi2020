# Lab 5 : Virtual memory ## Requirement 1 

## Requirement 1
### Translation Control Register (TCR)
 - [x] `required 1-1`  Set up TCR_EL1.
 - [x] `required 1-2`  Set up MAIR_EL1.
 - [x] `required 1-3`  Set up identity mapping.
 - [x] `required 1-4`  Modify linker script and map the upper address space.

 - [x] `question 1` Without indirect branch, the code might still work fine, why it’s the case and why it’s mandatory to use indirect branch.
```
 一開始執行是kernel code 所以可以看到實體位址
 但是一旦回到user mode 就會認不出來在哪裡 所以要用indirect branch
```
 - [x] `required 1-5` Linear map kernel with finer granularity and map RAM as normal memory.

 - [x] `question 2` For mapping 1GB memory region, how many page frames are used by page tables(PGD, PUD, PMD, and PTE) in four level translation?

```
page size 2^12 =  4096
1*1*1*256*4096 = 1048576 = 1GB
(1個PGD+1個PUD+1個PMD+256個PTE)
```

## Requirement 2
 - [x] `required 2-1` Implement page bookkeeping.
 - [x] `required 2-2` Implement the translation function between kernel virtual address, physical address, PFN and the reference to `struct page`.
 - [x] `required 2-3` Implement page_alloc and page_free.
 - [x] `question 3`   If a page frame is allocated and to be mapped at user space. Is it necessary to initialize it into 0?
```
需要
因為可能會被user指到 如果留下資料會有安全問題
```


## Requirement 3
 - [x] `required 3-1` Implement user space paging.
 - [x] `required 3-2` Implement shell as an user program and use objcopy to turn the ELF file into a raw binary..
 - [x] `required 3-2` Set TTBR0_EL1 to switch between different address space when context switch.


## Requirement 4

 - [x] `required 4-1` Implement page frame reclaim.