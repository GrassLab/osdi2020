# Lab0

第 0 個 lab 是環境設定，列出一些自己遇到的問題，我的系統是 `Ubuntu 18.04`

## Build

如果直接編譯不加任何 flag 會出現以下錯誤

```
aarch64-linux-gnu-gcc   -T linker.ld  boot.S   -o boot
/usr/lib/gcc-cross/aarch64-linux-gnu/7/../../../../aarch64-linux-gnu/lib/../lib/Scrt1.o: In function `_start':
(.text+0x18): undefined reference to `main'
(.text+0x1c): undefined reference to `main'
/usr/aarch64-linux-gnu/lib/libc_nonshared.a(elf-init.oS): In function `__libc_csu_init':
(.text+0xc): undefined reference to `__init_array_end'
(.text+0x10): undefined reference to `__init_array_start'
(.text+0x14): undefined reference to `__init_array_end'
(.text+0x18): undefined reference to `__init_array_start'
/usr/lib/gcc-cross/aarch64-linux-gnu/7/../../../../aarch64-linux-gnu/bin/ld: boot: hidden symbol `__init_array_end' isn't defined
/usr/lib/gcc-cross/aarch64-linux-gnu/7/../../../../aarch64-linux-gnu/bin/ld: final link failed: Bad value
collect2: error: ld returned 1 exit status
<builtin>: recipe for target 'boot' failed
make: *** [boot] Error 1
```

逆過自己寫的 C 程式就會知道經過 gcc 編譯後程式的進入點 `_start` 會先跑一些初始化的程式最後才會進到 `main` ，但我們目前的程式不需要那些東西，而且也沒有定義 `main` 就會造成編譯錯誤，加上 `-nostdlib` 或是 `-nostartfiles` 就能編譯成功

## QEMU

使用 `sudo apt install qemu` 會裝到 qemu 2.11 ，而這個版本還不能直接模擬 raspi3 (`-M raspi3`) 所以我到官網載最新的版本 (4.2.0) 自己編，不過需要裝一些 package:

```
sudo apt-get install build-essential zlib1g-dev pkg-config libglib2.0-dev binutils-dev libboost-all-dev autoconf libtool libssl-dev libpixman-1-dev libpython-dev python-pip python-capstone virtualenv
```

## Multi core

順利跑起來之後會看到 4 個 core 各自執行的指令，不過看了 linker.ld 以及 reference 會發現我們寫的程式應該會在 0x80000 但一開始的進入點在 0 和 0x300

> **Reset vector**
> In AArch64, the processor starts execution from an IMPLEMENTAION-DEFINED
> address, which is defined by the hardware input pins **RVBARADDR** and can be read by
> the RVBAR_EL3 register. You must place boot code at this address.
>
> - [Bare-metal Boot Code for ARMv8-A Processors](http://infocenter.arm.com/help/topic/com.arm.doc.dai0527a/DAI0527A_baremetal_boot_code_for_ARMv8_A_processors.pdf) P.36

根據手冊，CPU 第一個位址是由硬體定義的，在 GDB 上讀取 RVBAR_EL3 這個暫存器會得到 0 ，所以第一個執行的位址就是 0

```
(gdb) p $RVBAR_EL3
$8 = 0
(gdb)
```

位址 0 的 code 非常簡單

```
(gdb) x/10i 0
=> 0x0: ldr     x0, 0x18
   0x4: mov     x1, xzr
   0x8: mov     x2, xzr
   0xc: mov     x3, xzr
   0x10:        ldr     x4, 0x20
   0x14:        br      x4
(gdb) x/10g 0x18
0x18:   0x0000000000000100      0x0000000000080000
```

把位址 0x18 的值 (0x100) 給 x0，把 0x20 的值 (0x80000) 給 x4 然後跳去 x4，然後就會進到我們寫的 code 了，但不清楚為什麼要設定那個 `x0 = *0x18。另外也不知道其他 core 為什麼是從 0x300 開始跑，看了[這篇](https://www.design-reuse.com/articles/38128/method-for-booting-arm-based-multi-core-socs.html)之後大概能理解最終還是看 CPU 的硬體設定是會跳到哪個 address。以 qemu 模擬的 raspi3 來說 primary core 從 0 開始，其他的 core 從 0x300 開始。
