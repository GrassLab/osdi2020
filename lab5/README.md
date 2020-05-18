# NCTU OSDI LAB5
## 

| student id  | name  | lab  |
|---|---|---|
|  0510507 | 吳政緯  | 5  |

### Directory Tree

```
```

### Compiling
The `lab5_user` folder contains the test cases of user program. You have to compile the user program to raw binary and embed the compiled binary to kernel image.

```
$ cd    ./lab5_user/    &   make
```

Move the compiled `user_program` raw binary file into the kernel `src` folder. 

```
$ mv    user_program    ../src
```
Change directory to lab5 folder and make files.

```
$ cd .. &   make
```
## Running the tests

### run qemu
```
$ bash  start.sh
```
### run gdb
```
$ make  gdb
```
### recompile
```
$ make clean
```
