# My OSDI 2020

## Author

| 學號 | GitHub 帳號 | 姓名 | Email |
| --- | ----------- | --- | --- |
|`0856162`| `zodf0055980` | `吳宗遠` | zodf0055980@gmail.com |

## required 
### Install the cross compiler on your host computer.
sudo apt-get install binutils-aarch64-linux-gnu
sudo apt-get install gcc-aarch64-linux-gnu
### Install qemu
直接用 apt-get 的版本會是 2.11，而 qemu 要支援 raspi 3 要 [2.12](https://www.qemu.org/download/#source) 版以上，所以自己 make 一個來用

### Build your first kernel image and check it by QEMU.
我執行
```
.section ".text"
_start:
  wfe
  b _start
```
在 qemu 執行狀況會是
```
----------------
IN: 
0x00000000:  580000c0  ldr      x0, #0x18
0x00000004:  aa1f03e1  mov      x1, xzr
0x00000008:  aa1f03e2  mov      x2, xzr
0x0000000c:  aa1f03e3  mov      x3, xzr
0x00000010:  58000084  ldr      x4, #0x20
0x00000014:  d61f0080  br       x4

----------------
IN: 
0x00000300:  d2801b05  movz     x5, #0xd8
0x00000304:  d53800a6  mrs      x6, mpidr_el1
0x00000308:  924004c6  and      x6, x6, #3
0x0000030c:  d503205f  wfe      
0x00000310:  f86678a4  ldr      x4, [x5, x6, lsl #3]
0x00000314:  b4ffffc4  cbz      x4, #0x30c

----------------
IN: 
0x00080000:  d503205f  wfe      
0x00080004:  17ffffff  b        #0x80000

----------------
IN: 
0x0000030c:  d503205f  wfe      
0x00000310:  f86678a4  ldr      x4, [x5, x6, lsl #3]
0x00000314:  b4ffffc4  cbz      x4, #0x30c

----------------
IN: 
0x00000300:  d2801b05  movz     x5, #0xd8
0x00000304:  d53800a6  mrs      x6, mpidr_el1
0x00000308:  924004c6  and      x6, x6, #3
0x0000030c:  d503205f  wfe      
0x00000310:  f86678a4  ldr      x4, [x5, x6, lsl #3]
0x00000314:  b4ffffc4  cbz      x4, #0x30c

```
照理說執行完 0x00080004 會再跳回 0x80000 產生無窮迴圈
為何會跳到 0x0000030c 呢？ 是因為 wfe 指令會去等 event 進來嗎？
並且執行多次結果有可能不同

## question 
### What’s the RAM size of Raspberry Pi 3B+?
1GB SRAM
### What’s the cache size and level of Raspberry Pi 3B+?
直接去看 ortex-A53 (ARMv8) 64-bit SoC 的[規格書](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0500e/BABCFDAH.html)
共有 L1 和 L2 兩個 level，L1 32KB, L2 512KB.
### Explain each line of the above linker script.
```
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```
使用 objdump 觀察
```
(base) yuan@yuan-All-Series:~/Downloads$ objdump -h a.out 

a.out：     檔案格式 elf64-little

區段：
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001b  0000000000000200  0000000000000200  00000200  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  1 .note.ABI-tag 00000020  000000000000021c  000000000000021c  0000021c  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA

```
可以看到有兩個位置 vma lma
VMA (virtual memory address): 當 output file 運作時，section 會得到這個 address。
LMA (load memory address): 當 section 被載入 (loaded) 時，會放置到這個 address。
而我們可以透過告訴 linker 去擺放記憶體位置
```=
SECTIONS 　　　　＃　SECTIONS keyword 來表示開始宣告 SECTIONS。
{
  . = 0x80000; 　　　＃　location counter
  .text : { 
      *(.text) 
    }
      
}
```
location counter 表示目前的 location，當從 SECTIONS 開始的時候，其初始值為 0，location counter 會隨著 output section 的增加而增加，並且可以對 location counter 賦值。
2. 就是將 location counter 移動到(賦值) 0x10000。
4. 定義一個 output section .text在大括號裏面，可以擺放想要放入這個 section 的 contents。
5. 將所有 input files 的 .text input section 放入這個地方。* 是一個 wildcard 符號，會比對所有符合的檔名。

# reference
[link-script](https://blog.louie.lu/2016/11/06/10%E5%88%86%E9%90%98%E8%AE%80%E6%87%82-linker-scripts/)
[wfe](https://www.itread01.com/content/1548469269.html)

