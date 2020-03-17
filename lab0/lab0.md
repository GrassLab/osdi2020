# OSID Lab0

## install corss-compiler

```shell=
sudo apt-get install g++-aarch64-linux-gnu
```

## qusetion
+ What’s the RAM size of Raspberry Pi 3B+?
	+ 1GB SRAM
+ What’s the cache size and level of Raspberry Pi 3B+?
	+ 2 level cache
	+ 512 KB shared L2 cache
	+ 32kB L1 cache

```shell=
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```

+ Explain each line of the above linker script
	+ line 1: use SECTIONS keyword to start declare SCTIONS
	+ line 3: "." is location counter which means the location now. We set current location to 0x80000
	+ line 4: Define a output section ".text" and put all the .text input section in input files to here. "*" is a wildcard sign.

## install qemu

+ downlod latest releases qemu from [qemu.org](https://www.qemu.org/)
+ unzip downloaded file and cd into it
+ Then do the following command

```shell=
$ ./configure --target-list=arm-softmmu,aarch64-softmmu
$ make -j$(nproc)
$ make install
```

+ finish

## Write a.S

```shell=
.section ".text"
_start:
  wfe
  b _start
```

+ Assemble the assembly
```shell=
aarch64-linux-gnu-gcc -c a.S
```

## Write link.ld

```shell=
SECTIONS
{
  . = 0x80000;
  .text : { *(.text) }
}
```

## From object files to ELF

```shell=
$ aarch64-linux-gnu-ld -T link.ld -o kernel8.elf a.o
```

## From elf to kernel image

```shell=
$ aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
```

## Check on QEMU

```shell=
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
```
