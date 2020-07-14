#include "tmpfs.h"
#include "uart.h"
#include "my_string.h"

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name){
    struct tmpfs_dir_struct* node = dir_node->internal;
    /*uart_puts("Node file name: ");
    uart_puts(node->file[0]->name);
    uart_puts("\n");
    uart_puts("Component_name: ");
    uart_puts(component_name);
    uart_puts("\n");*/
    for (int i=0; i<node->file_num; i++){
        //if (node->file[i]->valid == 1) uart_puts("OKKKKKKKKK!\n");
        //if (!strcmp(node->file[i]->name, component_name)) uart_puts("COOOLLLLLLLLLLLLL!\n");
        if (node->file[i]->valid == 1 && strcmp(node->file[i]->name, component_name)){
            (*target)->internal = node->file[i];
            return 0;
        }
    }
    //uart_puts("Can not find the file: ");
    //uart_puts(component_name);
    //uart_puts("\n");
    return 1;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target, const char* component_name){
    struct tmpfs_dir_struct* node = dir_node->internal;
    for (int i=0; i<TMPFS_FILE_NUM; i++){
        if (file_pool[i].valid == 0){
            node->file[node->file_num] = &file_pool[i];
            file_pool[i].valid = 1;
            node->file_num++;
            int c=0;
            for (; component_name[c] != '\0'; c++) file_pool[i].name[c] = component_name[c];
            file_pool[i].name[c] = '\0';
            //uart_puts(file_pool[i].name);
            //uart_puts("\n");
            for (int j=0; j<VNODE_POOL_SIZE; j++){
                if (vnode_pool[j].valid == 0){
                    vnode_pool[j].valid = 1;
                    vnode_pool[j].f_ops = dir_node->f_ops;
                    vnode_pool[j].v_ops = dir_node->v_ops;
                    vnode_pool[j].type = 1;
                    vnode_pool[j].internal = &file_pool[i];
                    (*target) = &vnode_pool[j];
                    //uart_puts(((struct tmpfs_file_struct*)(vnode_pool[j].internal))->name);
                    return 0;
                }
            }
        }
    }
    return 1;
}