# Lab1

## Basic initialization
1. Let only one core proceed, and let others enter a busy loop.
2. Initialize the BSS segment.
3. Set the stack pointer to an appropriate position.

### Let only one core proceed, and let others enter a busy loop.
+ 讓一個core去處理，首先先讀取mpidr_el1這個register，這個register會給processor，由於只要讓一個運行，所以我們判別是否為0，如果是0就讓他執行，不是就進到無窮迴圈(wfe)。

```shell=
.section ".text"
_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
1:  wfe
    b 1b
2:  //start run your code in thread 1

    b 1b //after finished jump back to wfe
```

### Initialize the BSS segment.

+ 若有不懂的部分可以google搜尋 加上 site:ftp.gnu.org
這個網站有很多解釋

+ 根據這個memory layout我們的main要執行時要先幫他分好bss以及stack還有其他像是rodata data等section。不一定要照順序擺，隨便你怎麼放也可以。
![](https://i.imgur.com/1GrjmFV.png)

+ 首先我們先將bss段清成0，延續上面的code
```shell=
.section ".text"
_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
1:  wfe
    b 1b
2:  ldr x1, =__bss_start
    ldr w2, =__bss_size
3:  cbz x2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b
4:  //start run your code in thread 1
    b 1b //after finished jump back to wfe
```
從label 2開始，就是單一core執行，要清空bss段首先我們要先拿到bss段的位址，這裡\_\_bss_start以及\_\_bss_size這兩個都是label會由linker給他們實際位址，所以暫且就先當作我們將bss段的開始以及大小分別放入x1,w2這兩個register。

接著我們要一個一個將這段記憶體的位址清空，這裡我們用w2當作counter來計算，```cbz w2,4f``` 表示如果w2為0就跳離迴圈到label 4開始執行main，所以表示bss已清空完成。若尚未清空則執行```str xzr, [x1], #8```這裡str是指將xzr這個0的register的值寫入到x1 register裡指向的地方，接著將x1裡的address加8。(其中還有另一種用法是str xzr, [x1, #8]，表示直接將0存進x1+8的address裡)，最後將w2-1(```sub w2, w2, #1```)，這裡減一是因為我們linker裡的\_\_bss_size有先將該值除8(>>3)所以只要-1不然是-8。最後比對是否為0，否則就跳回label 3執行。

+ 將BSS段清成0，首先我們先在linker script當中定義出bss段的位置，如下:
```shell=
SECTIONS
{
    . = 0x80000;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r*) }
    PROVIDE(_data = .);
    .data : { *(.data .data.* .gnu.linkonce.d*) }
    .bss (NOLOAD) : {
        . = ALIGN(16);
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;

   /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
__bss_size = (__bss_end - __bss_start)>>3;
```
上述link.ld解釋如下:
8. 這裡開始定義BSS段，NOLOAD表示運行時不會被載入記憶體，但是linker在這裡仍然會正常處理這一段。
9. ALIGN(16)對齊記憶體位址
10. 新增一個變數 \_\_bss_start數值為目前位置，所以這裡是開始bss段的位置
11. 12. 載入所有.o檔要放在bss段的東西
13. bss段結束一樣assign一個\_\_bss_end用來計算後面的size
15. 整個linker結束
17. 把一些elf資訊(編譯器平台等資訊)拿掉 不link的意思
19. 計算bss大小，之後拿來清空，這裡>>3將大小向右位移是因為記憶體一次存取是8個byte一起存取，所以在後面會看到assembly檔案清空時會拿register的值+8，把0塞進去。

others: 

1. ".gnu.linkonce"這些section是g\+\+用的，g\+\+會把每個template做擴充然後放在自己的sction裡，這些會被定義成弱符號，所以可以多個定義，所以GNU裡的linker實際上只會用一個其它丟掉。
[source](https://gcc.gnu.org/legacy-ml/gcc/2003-09/msg00984.html)
2. KEEP: 當有開啟 link-time garbage collection 的時候，有時候我們希望某個區段不要被回收掉，這時候就可以使用 KEEP，使用方式為將 input section wildcard entry 放入 KEEP 中。e.g. KEEP(*(.init)) 或是 KEEP(SORT_BY_NAME(*)(.ctors)) [source](https://blog.louie.lu/2016/11/06/10%E5%88%86%E9%90%98%E8%AE%80%E6%87%82-linker-scripts/)
3. 簡單來說，如果你的程式已經有這個symbol（函數或變數），就用你的；否則就使用這邊提供的symbol。[source](http://wen00072.github.io/blog/2014/03/14/study-on-the-linker-script/#assign-prov)
4. \*(COMMON)表示文件當中所有未初始化(uninitialized)的數據 

### Set the stack pointer to an appropriate position.

從上面那張memory layout的圖可以看出stack是在text段的上面，所以這裡我們直接將sp指向_start的位置，接下來就sp就會由高到低位長過去。

```shell=
ldr x1, =_start
mov sp, x1
```

所以最後做完完整的start.S code就如下:
```shell=
.section ".text.boot"
_start:
    mrs x1, mpidr_el1
    and x1, x1, #3
    cbz x1, 2f
1:  wfe
    b 1b
2:  ldr x1, =_start
    mov sp, x1
    ldr x1, =__bss_start
    ldr w2, =__bss_size
3:  cbz w2, 4f
    str xzr, [x1], #8
    sub w2, w2, #1
    cbnz w2, 3b
4:  b1 main
    b 1b
```

## Mini UART

這裡我直接照著Bare metal Raspberry Pi 3 tutorials的 github寫，
前段有大致上看一下，後面有個"map UART1 to GPIO pins" 不加qemu可以跑起來，可是實體跑步起來，看起來似乎就如他註解所述。上半段"initialize UART"的部分有就照著網站給的設。

```c=
void uart_init()
{
    register unsigned int r;

    /* initialize UART */
    *AUX_ENABLE |=1;       // enable UART1, AUX mini uart
    *AUX_MU_CNTL = 0;      // Disable transmitter and receiver during configuration.
    *AUX_MU_IER = 0;       // Disable interrupt because currently you don’t need interrupt.
    *AUX_MU_LCR = 3;       // 8 bits 一次傳輸的最小單位
    *AUX_MU_MCR = 0;       // Don’t need auto flow control. 控制速度
    *AUX_MU_BAUD = 270;    // 115200 baud    
    *AUX_MU_IIR = 0x6;    // disable interrupts

    /* map UART1 to GPIO pins */
    r=*GPFSEL1;
    r&=~((7<<12)|(7<<15)); // gpio14, gpio15
    r|=(2<<12)|(2<<15);    // alt5
    *GPFSEL1 = r;
    *GPPUD = 0;            // enable pins 14 and 15
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = (1<<14)|(1<<15);
    r=150; while(r--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;        // flush GPIO setup
    *AUX_MU_CNTL = 3;      // enable Tx, Rx
}
```

內容也寫好了讀取跟寫入，基本上就是看訊號是否有接收到。

## Simple Shell

就一般寫C的方式，這裡有特別把uart_getc包成myread function，同時這裡我也可以使用方向鍵以及刪除中間或插入。
當中會需要用到一些字串處理，去網路上搜尋implement copy就好XD

## Get time

這部分滿特別的，可以參考Bare metal Raspberry Pi 3 tutorials的07_delays，一開始我原本打算用讀取後用rsp + offset的方式把值存到變數，後來看到c裡面這種神奇的asm寫法```asm volatile ("mrs %0, cntpct_el0" : "=r"(r));```，他可以將變數map到某個register然後存到變數裡面，其中r就是在c裡面宣告的變數，記得加volatile避免被優化掉就好。(如果沒加volatile放到迴圈裡 數值不會變 因為他只存第一次 之後因為都是迴圈裡 所以她不會更動..被優化了)。
解決讀取coprocesser register的問題後，就做浮點數運算將時間顯示即可，這裡我用稍微演算法的方式，原本我一開始用float去分出整數小數，可是如果小數1.012345這種小數點後0開頭的，當\*10000時0會被忽略變成1.12345這樣就不精確，所以改用類似算2進位小數的方式改成10進位去算，最後整數小數concat起來就好。

## Reboot

這部分照著寫即可。