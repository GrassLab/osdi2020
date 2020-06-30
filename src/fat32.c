#include "vfs.h"
#include "fat32.h"
#include "mm.h"
#include "sdhost.h"
#include "uart0.h"

struct vnode_operations* fat32_v_ops = NULL;
struct file_operations* fat32_f_ops = NULL;

// error code: -1: already register
int fat32_register() {
    if (fat32_v_ops != NULL && fat32_f_ops != NULL) {
        return -1;
    }
    fat32_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
    fat32_v_ops->create = fat32_create;
    fat32_v_ops->lookup = fat32_lookup;
    fat32_v_ops->ls = fat32_ls;
    fat32_v_ops->mkdir = fat32_mkdir;
    fat32_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));
    fat32_f_ops->read = fat32_read;
    fat32_f_ops->write = fat32_write;
    return 0;
}

int fat32_setup_mount(struct filesystem* fs, struct mount* mount) {
    // get metadata
    // readblock(0, buf);
    // for (int i = 0; i < 512 ;i++) {
    //     uart_printf("%d %x\n", i, buf[i]);
    // }

    return 0;
}

int fat32_lookup(struct vnode* dir, struct vnode** target, const char* component_name) {
    
}

int fat32_create(struct vnode* dir, struct vnode** target, const char* component_name) {

}

int fat32_ls(struct vnode* dir) {

}

int fat32_mkdir(struct vnode* dir, struct vnode** target, const char* component_name) {

}

// file operations
int fat32_read(struct file* file, void* buf, uint64_t len) {

}

int fat32_write(struct file* file, const void* buf, uint64_t len) {

}