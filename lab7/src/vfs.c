#include "vfs.h"
#include "my_string.h"

char fs_list[10][10];
int use_fs_num = 0;

int register_filesystem(struct filesystem* fs) {
    for (int i=0; fs->name[i] != '\0'; i++){
        fs_list[use_fs_num][i] = fs->name[i];
    }
    use_fs_num++;
}