# Lab6 Questions

> Q: Is buddy allocator perfectly eliminate external fragmentation? If yes, prove it? If no, give an external fragmentation example.
A: No. Allocate 4 order 0 block size. Free the two of them. They won't be able to merge because the remaining two buddy is not currently free.

> Q: If the registered object size is 2049 byte, one page frame can only fit in one object. Hence the internal fragmentation is around 50%. How to decrease the percentage of the internal fragmentation in this case?
A: Use larger order size block for continuous allocation.

> Q: What’s the benefit to prevent static allocation?
A: Smaller executable size. Ability to dynamic decide the space needed

