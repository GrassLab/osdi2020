# My OSDI 2020

## Author

| Student ID | GitHub Account | Name | Email                      |
| -----------| -------------- | ---- | -------------------------- |
| 0856009    | JingWangTW     | 王靖  | jingwang.cs08g@nctu.edu.tw |

## Files
| File          | Content      | 
| --------------| ------------ | 
| lab0.ld       | linker script|
| lab0.s        | source code  |

## How to build

### Get object file from source code
```bash
aarch64-linux-gnu-gcc -c lab0.s -o lab0.o
```

### Get elf from object file
```bash
aarch64-linux-gnu-ld -T lab0.ld -o kernel8.elf lab0.o
```

### Get image file form elf
```bash
aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img
```

## Run on QEMU
```bash
qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
```
