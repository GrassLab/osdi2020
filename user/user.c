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

//     int root = open("/", 0);
//     readdir(root);
// }

void demo_lab7_ele3() {
    mkdir("mnt");
    int root = open("/", 0);
    readdir(root);
    chdir("mnt");
    int b = open("world", O_CREAT);
    readdir(root);
    int mnt = open("/mnt", 0);
    readdir(mnt);
}

int main() {
    // while (1) {
    //     char cmd[64];
    //     shell_input(cmd);
    //     shell_controller(cmd);
    // }
    demo_lab7_ele3();
}