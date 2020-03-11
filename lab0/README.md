# Lab0(2020/03/04-2020/03/11)
Question:

What's the RAM size of Raspberry Pi 3B+

1GB LPDDR2 SDRAM

[reference]

[Raspberry Pi 3 Model B+](https://www.raspberrypi.org/products/raspberry-pi-3-model-b-plus/)


Question:

What's the cache size and level of Raspberry Pi 3B+

Level1 cache 32KB

Level2 cache 512KB

Originally I try to find cache size by using 'getconf -a | grep CACHE' and 'lscpu | grep cache' on raspbian, but the output is always 0, this section will update after further experiment.


## 0x00 - Install cross compiler(required)

There are several cross compiler for compiling a aarch64 archetecture program, I choose to use [linaro toolchain](https://www.linaro.org/downloads/).

In it's toolchain there are two version bare-metal and linux targeted binar release, since our labs require to write bare-metal in rpi3 b+ we need to install bare-metal version. These compiler are precompiled binary so add the directory into $PATH then we complete first step of lab0

