#include "vfs.h"
#include "uart.h"
#include "my_string.h"

//char fs_list[10][10];
use_fs_num = 0;

struct file fd_pool[FD_POOL_SIZE] __attribute__((aligned(16u))) = {{.valid = 0}}; 

int register_filesystem(struct filesystem* fs) {
    for (int i=0; fs->name[i] != '\0'; i++){
        fs_list[use_fs_num][i] = fs->name[i];
    }
    use_fs_num++;
}

struct file* vfs_open(const char* pathname, int flags){
    char compo_name[50][50];
    int num = 0, j = 0;
    int i = (pathname[0] == '/') ? 1 : 0;

    for (; pathname[i] != '\0'; i++){
        if (pathname[i] == '/'){
            compo_name[num][j] = '\0';
            num++;
            j = 0;
            continue;
        }
        compo_name[num][j++] = pathname[i];
    }

    compo_name[num][j] = '\0';

    /*for (int k=0; k<=num; k++){
        uart_puts(compo_name[k]);
        uart_puts("\n");
    }*/
    struct vnode* now = rootfs->root;
    int found ;
    for (i = 0; i<=num; i++){
        found = !(now->v_ops->lookup(now, &now, compo_name[i]));
        if (!found) break;
    }

    if (found){
        for (i=0; i<FD_POOL_SIZE; i++){
            if (fd_pool[i].valid == 0){
                fd_pool[i].vnode = now;
                fd_pool[i].flags = flags;
                fd_pool[i].valid = 1;
                return &fd_pool[i];
            }
        }
    }
    else{
        if (flags == O_CREAT){
            now->v_ops->create(now, &now, compo_name[num]);

            for (i=0; i<FD_POOL_SIZE; i++){
                if (fd_pool[i].valid == 0){
                    fd_pool[i].vnode = now;
                    fd_pool[i].flags = flags;
                    fd_pool[i].valid = 1;
                    uart_puts("Create file: ");
                    uart_puts(compo_name[num]);
                    uart_puts("\n");
                    return &fd_pool[i];
                }
            }
        }
        else return 0;
    }
}

int vfs_close(struct file* file){
    file->flags = 0;
    file->vnode = 0;
    file->f_pos = 0;
    file->valid = 0;
    
    return 0;
}