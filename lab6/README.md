# Lab 6 : Allocator

## Buddy System

- [ ] `required 1` Implement the buddy system for contiguous pages allocation.
- [ ] `question 1` Is buddy allocator perfectly eliminate external fragmentation? If yes, prove it? If no, give an external fragmentation example.

## Object Allocator

- [ ] `required 2-1` Implement the API for register a fixed-size allocator.
- [ ] `required 2-2` Implement the API for allocate and free for fixed-size object.

- [ ] `question 2` If the registered object size is 2049 byte, one page frame can only fit in one object. Hence the internal fragmentation is around 50%. How to decrease the percentage of the internal fragmentation in this case?

## Varied-sized allocator

- [ ] `required 3` Implement a varied-sized allocator.

- [ ] `question 3` What’s the benefit to prevent static allocation?