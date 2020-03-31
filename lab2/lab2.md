# Lab 2

## Mailbox

+ mailbox.c code
```c=
//傳入mail訊息內容的address 以及 channel number
int mbox_call(unsigned int *mailbox, unsigned char ch)
{
    //將mailbox訊息的address的upper 28bit 以及 lower 4 bit的channel傳入這裡用&的方式當mask過濾出來
    unsigned int reg = (((unsigned int)((unsigned long)mailbox)&~0xF) | (ch&0xF));
    //檢查Mailbox0 status的register的full flag有沒有滿 滿的話就先loop
    do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_FULL);
    //將資料寫入address
    *MAILBOX_WRITE = reg;
    while(1){
        //等待回應，看Mailbox0 status的register是不是empty
        do{asm volatile("nop");}while(*MAILBOX_STATUS & MAILBOX_EMPTY);
        //如果不是empty就比對該訊息是不是我們要讀取的同一個address
        if(reg == *MAILBOX_READ)
        {   //看回傳值有沒有設
            return mailbox[1]==MBOX_RESPONSE;
        }
    }
    return 0;
}
```

## Get the hardware’s information

+ 這裡依照參考的網站，看要在mailbox設定什麼值，去拿到board revision and VC Core base address.
![](https://i.imgur.com/x9UjV4y.png)
![](https://i.imgur.com/yDAn1Mf.png)
+ 設定的順序是:
    + [0] buffer size in bytes
    + [1] request or response code
    + [2] tag identifier
    + [3] request length (若無則省略 改為 response length)
    + [4] response length (同上)
    + [5~n] ... 要帶入的參數以此後加
    + [n+1] 最後放上 0 表示 ENG Tag
    (這裡另外要注意 要把response 會放的buffer位置 清成0 向底下[5][6] 就是會回傳的)
+ VC base address
```c=
unsigned int  mailbox[36];
mailbox[0] = 7 * 4; // buffer size in bytes
mailbox[1] = REQUEST_CODE;
// tags begin
mailbox[2] = 0x00010006; // tag identifier
mailbox[3] = 8; // maximum of request and response value buffer's length.
mailbox[4] = TAG_REQUEST_CODE;
mailbox[5] = 0; // clear buffer
mailbox[6] = 0;
// tags end
mailbox[7] = END_TAG;
mbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
mywrite("VC memory base address in bytes: ");
uart_hex(mailbox[5]);
mywrite("\nVC memory size in bytes: ");
uart_hex(mailbox[6]);
mywrite("\n");
```

+ board revision
```c=
unsigned int  mailbox[36];
mailbox[0] = 7 * 4; // buffer size in bytes
mailbox[1] = REQUEST_CODE;
// tags begin
mailbox[2] = GET_BOARD_REVISION; // tag identifier
mailbox[3] = 4; // maximum of request and response value buffer's length.
mailbox[4] = TAG_REQUEST_CODE;
mailbox[5] = 0; // value buffer
// tags end
mailbox[6] = END_TAG;
mbox_call(mailbox, MBOX_CH_PROP); // message passing procedure call, you should implement it following the 6 steps provided above.
mywrite("board revision: ");
uart_hex(mailbox[5]);
mywrite("\n");
```
## PL011 UART

+ code中和mini UART當時所設的map UART0 to GPIO pins部份一樣，其他則是由mailbox去設定UART clock之後再回來計算

+ 首先一樣使用上面的mailbox去設UART clock
![](https://i.imgur.com/BZtDfSO.png)

+ 這部分就是設好set clock的tag和其他資料如上圖及下面程式碼
```c=
unsigned int  mbox[36];
    /* set up clock for consistent divisor values */
mbox[0] = 9*4;
mbox[1] = MBOX_REQUEST;
mbox[2] = MBOX_TAG_SETCLKRATE; // set clock rate
mbox[3] = 12; //request 長度12
mbox[4] = 8;
mbox[5] = 2;           // UART clock
mbox[6] = 4000000;     // 4Mhz
mbox[7] = 0;           // clear turbo
mbox[8] = MBOX_TAG_LAST;
mbox_call(mbox, MBOX_CH_PROP);
```

+ 接著就是設baud rate以及map UART0的pin，其中map UART0就跟之前一樣就不放了

```c=
#define UART0_DR        ((volatile unsigned int*)(MMIO_BASE+0x00201000))
#define UART0_FR        ((volatile unsigned int*)(MMIO_BASE+0x00201018))
#define UART0_IBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201024))
#define UART0_FBRD      ((volatile unsigned int*)(MMIO_BASE+0x00201028))
#define UART0_LCRH      ((volatile unsigned int*)(MMIO_BASE+0x0020102C))
#define UART0_CR        ((volatile unsigned int*)(MMIO_BASE+0x00201030))
#define UART0_IMSC      ((volatile unsigned int*)(MMIO_BASE+0x00201038))
#define UART0_ICR       ((volatile unsigned int*)(MMIO_BASE+0x00201044))

*UART0_ICR = 0x7FF;    // clear interrupts
*UART0_IBRD = 2;       // 115200 baud
*UART0_FBRD = 0xB;
*UART0_LCRH = 0b11<<5; // 8n1
*UART0_CR = 0x301;     // enable Tx, Rx, FIFO
```

+ 當中IBRD的算法以及FBRD的算法如下:
    + $clockrate$是mailbox中所設的
    + $baud$是baud rate
    + $temp$是整數
    
$uart0\_ibrd = \dfrac{clockrate}{(16 * baud)}$

$temp = \dfrac{((clockrate\% (16 * baud)) * 8)}{baud}$

$uart0\_fbrd = (temp>>1) + (temp\&1)$

## Splash

+ 這裡就依照github上的內容照用..
    + 首先就先設定物理的輸出大小，1024*768
    + 再設定虛擬的輸出大小 跟物理大小一樣
    + 接著就是 畫圖的offfset 就是從哪裡開始畫
    + 再來是深度，也就是說 一個像素要幾個bit 這裡是32個
    + 再來就是order,RGB or BGR
    + 然後allocate一塊frame buffer(1 row)
    + 最後取得pitch 得到一個row的長度。(一個row有幾個bytes)
+ 以上mailbox設好後，後面我們就取得回傳值。接著就依序填入像素
```c=
void lfb_init()
{
    unsigned int mbox[36];
    mbox[0] = 35*4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003;  //set phy wh 物理大小
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;         //FrameBufferInfo.width
    mbox[6] = 768;          //FrameBufferInfo.height

    mbox[7] = 0x48004;  //set virt wh 虛擬大小
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;        //FrameBufferInfo.virtual_width
    mbox[11] = 768;         //FrameBufferInfo.virtual_height

    mbox[12] = 0x48009; //set virt offset 起始位置
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;           //FrameBufferInfo.x_offset
    mbox[16] = 0;           //FrameBufferInfo.y.offset

    mbox[17] = 0x48005; //set depth 深度 一個pixel的bits
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          //FrameBufferInfo.depth

    mbox[21] = 0x48006; //set pixel order 顏色順序
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           //RGB, not BGR preferably

    mbox[25] = 0x40001; //get framebuffer, alignment on request 設定buffer
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        //FrameBufferInfo.pointer
    mbox[29] = 0;           //FrameBufferInfo.size

    mbox[30] = 0x40008; //get pitch 一個row的長度(bytes)
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           //FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    //this might not return exactly what we asked for, could be
    //the closest supported resolution instead
    if(mbox_call(mbox, MBOX_CH_PROP) && mbox[20]==32 && mbox[28]!=0) {
        mbox[28]&=0x3FFFFFFF;   //convert GPU address to ARM address
        width=mbox[5];          //get actual physical width
        height=mbox[6];         //get actual physical height
        pitch=mbox[33];         //get number of bytes per line
        isrgb=mbox[24];         //get the actual channel order
        lfb=(void*)((unsigned long)mbox[28]);
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}
```

## Load kernel image
+ 這裡也是依照github去寫，方法是start.S一樣會先載入到0x80000的位置，但是在載入後我們會把它移動到linker.ld裡面寫的我們要放的_start的位置，所以這裏的做法會需要有整個的size,那在linker裡面我們用__loader_size來表示 

```c=
.section ".text.boot"

.global _start

_start:
    // save arguments in registers (we will need them later for the new kernel)
    // I choosed x10-x13 because instructions generated from C by gcc does not
    // touch them. You can check that with "aarch64-elf-objdump -d kernel8.elf"
    mov     x10, x0
    mov     x11, x1
    mov     x12, x2
    mov     x13, x3

    // read cpu id, stop slave cores
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    // cpu id > 0, stop
1:  wfe
    b       1b
2:  // cpu id == 0

    // relocate our code from load address to link address
    ldr     x1, =0x80000
    ldr     x2, =_start
    ldr     w3, =__loader_size
1:  ldr     x4, [x1], #8
    str     x4, [x2], #8
    sub     w3, w3, #1
    cbnz    w3, 1b

    // set stack before our code
    ldr     x1, =_start
    mov     sp, x1

    // clear bss
    ldr     x1, =__bss_start
    ldr     w2, =__bss_size
3:  cbz     w2, 4f
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b

    // jump to relocated C code, should not return
4:  bl      main-2048
    // for failsafe, halt this core too
    b       1b
```
+ 這裏 code和lab1一樣，差別在於，中間加入一段relocate的code
+ 由於預設載入就是在0x80000所以我們這裏就把code移動到0x80000之前（_start在linker.ld裡面寫的位置）
+ 所以這裡我們把code移動到0x80000之前也就是我們的原本stack段，如此我們就可以將要load的kernal載入到0x80000的位置也不會碰到我們的loader code