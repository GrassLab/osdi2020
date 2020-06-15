# Lab 6 : Allocator

## Buddy System

- [x] `required 1` Implement the buddy system for contiguous pages allocation.
- [x] `question 1` Is buddy allocator perfectly eliminate external fragmentation? If yes, prove it? If no, give an external fragmentation example.
如果很多小物件宣告、但是都沒有連續歸還，會讓上面的一直找不到buddy，還是會有exteral
50%

## Object Allocator

- [x] `required 2-1` Implement the API for register a fixed-size allocator.
- [x] `required 2-2` Implement the API for allocate and free for fixed-size object.

- [x] `question 2` If the registered object size is 2049 byte, one page frame can only fit in one object. Hence the internal fragmentation is around 50%. How to decrease the percentage of the internal fragmentation in this case?

1.可以有機制去搜尋內部有多少是不會用到的 可以拿去分配
2.不要用page_frame限定一個object 而是用allocator

a: 多要幾塊連續的page 一多要幾塊

## Varied-sized allocator

- [x] `required 3` Implement a varied-sized allocator.

- [x] `question 3` What’s the benefit to prevent static allocation?
如果很少使用的一個固定size 跟page_allocator拿了一些page，可能會很少用到而閒置。
a:卡在bss 所以能掌握的就是固定的 無法讓別人使用