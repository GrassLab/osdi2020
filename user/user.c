#include "user_lib.h"
// #include "shell.h"

// void demo_lab7_ele2() {
//     int a = open("hello", O_CREAT);
//     int b = open("world", O_CREAT);
//     write(a, "Hello ", 6);
//     write(b, "World!", 6);
//     close(a);
//     close(b);
//     b = open("hello", 0);
//     a = open("world", 0);
//     int sz;
//     char buf[32];
//     sz = read(b, buf, 100);
//     sz += read(a, buf + sz, 100);
//     buf[sz] = '\0';
//     printf("%s\n", buf); // should be Hello World!
// }

void demo_lab7_ele3() {
    char buf[8];
    mkdir("mnt");
    int fd = open("/mnt/a.txt", O_CREAT);
    write(fd, "Hi", 2);
    close(fd);
    chdir("mnt");
    fd = open("./a.txt", 0);
    read(fd, buf, 2);
    printf("1: %s\n", buf);

    chdir("..");
    mount("tmpfs", "mnt", "tmpfs");
    fd = open("mnt/a.txt", 0);
    printf("2: %d\n", fd);

    umount("/mnt");
    fd = open("/mnt/a.txt", 0);
    printf("3: %d\n", fd);
    read(fd, buf, 2);
    printf("4: %s\n", buf);
}

int main() {
    // while (1) {
    //     char cmd[64];
    //     shell_input(cmd);
    //     shell_controller(cmd);
    // }
    demo_lab7_ele3();
}