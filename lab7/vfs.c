#include "vfs.h"
#include "mystd.h"
#include "uart.h"

struct mount rootfs;

struct dentry dentryObj[64];
int dentryObjNum;
struct vnode vnodeObj[64];
int vnodeObjNum;
struct file_descriptor fdObj[64];
int fdObjNum;

struct file_descriptor_operations GLOBAL_fd_ops;
struct dentry_operations GLOBAL_dentry_ops;

int setup_mount(struct filesystem *fs, struct mount *mount){
    mount->root = fs->root;
    mount->fs = fs;
    return 1;
}

int lookup(struct dentry* dir_node, struct dentry** target, const char* component_name){
    if(dir_node->count==0) { uart_puts("count zero\n"); return 0;}
    for(int i=0; i<dir_node->count; i++){
        struct entry_list *list_node = &dir_node->d_list[i]; 
        if(list_node->used==1 && strcmp(list_node->child->name, (char*)component_name)){
            *target = list_node->child;
            return 1;
        }
    }

    return 0;
}

int create(struct dentry* parent_dentry, struct dentry **target, const char* component_name, enum filetype type){
    struct dentry *new_dentry = &dentryObj[dentryObjNum];
    dentryObjNum++;
    for(int i=0; i<32; i++) new_dentry->name[i] = '\0';
    strcpy(new_dentry->name, (char*)component_name);
    new_dentry->parent = parent_dentry;
    new_dentry->type = type;
    new_dentry->d_ops->lookup = lookup;
    new_dentry->d_ops->create = create;

    if(type == file){
        struct vnode *new_vnode = &vnodeObj[vnodeObjNum];
        vnodeObjNum++;
        for(int i=0; i<1024; i++) new_vnode->mem[i] = '\0';

        new_dentry->vnode = new_vnode;
    }else{
        new_dentry->count = 0;
    }

    // add to parent's child
    parent_dentry->d_list[parent_dentry->count].used = 1;
    parent_dentry->d_list[parent_dentry->count].child = new_dentry;
    parent_dentry->count++;

    *target = new_dentry;

    return 1;
}

int fd_write(struct file_descriptor* fd, char* buf, int len){
    int fd_start = fd->f_pos;

    for(int i=0; i<len; i++){
        fd->vnode->mem[fd_start + i] = buf[i];
    }


    return 1;
}

int fd_read(struct file_descriptor *fd, char *buf, int len){
    int fd_start = fd->f_pos;
    int size = 0;

    for(size=0; size<len; size++){
        if(fd->vnode->mem[fd_start + size] == '\0') break;
        buf[size] = fd->vnode->mem[fd_start + size];
    }

    return size;
}

int register_filesystem(struct filesystem *fs){
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount;

    // init memory
    dentryObjNum = 0;
    vnodeObjNum = 0;
    fdObjNum = 0;

    GLOBAL_fd_ops.write = fd_write;
    GLOBAL_fd_ops.read = fd_read;
    GLOBAL_dentry_ops.lookup = lookup;
    GLOBAL_dentry_ops.create = create;

    // create root
    struct dentry *tmpfs_root = &dentryObj[dentryObjNum];
    dentryObjNum++;
    tmpfs_root->name[0] = '/';
    tmpfs_root->name[1] = '\0'; 
    tmpfs_root->parent = NULL;
    tmpfs_root->count = 0;
    tmpfs_root->d_ops = &GLOBAL_dentry_ops;
    fs->root = tmpfs_root;

    // setup as root file system
    fs->setup_mount = setup_mount;
    fs->setup_mount(fs, &rootfs);

    return 1;
}



struct file_descriptor* create_fd(struct dentry *dentry){
    struct file_descriptor *fd = &fdObj[fdObjNum];
    fdObjNum++;

    fd->vnode = dentry->vnode;
    fd->f_pos = 0;
    fd->fd_ops = &GLOBAL_fd_ops;

    return fd;
}

struct file_descriptor* vfs_open(const char* pathname, int flags) {
    struct dentry *root = rootfs.root;
    struct dentry *find_entry = NULL;

    if(0 == root->d_ops->lookup(root, &find_entry, pathname)) {
        if(flags == O_CREAT){
            root->d_ops->create(root, &find_entry, pathname, file);
            uart_puts("[vfs_open] lookup fail, create new dentry\n");
        }else{
            uart_puts("[vfs_open] lookup fail, return NULL\n");
            return NULL;
        }
    }else{
        uart_puts("[vfs_open] lookup success\n");
    }


    struct file_descriptor *fd = create_fd(find_entry);

    return fd;
}

void vfs_close(struct file_descriptor *fd){
    fd->vnode = NULL;
}

int vfs_read(struct file_descriptor *fd, char *buf, int len){
    return fd->fd_ops->read(fd, buf, len);
}

void vfs_write(struct file_descriptor *fd, char *buf, int len){
    fd->fd_ops->write(fd, buf, len);
}

/*
int vfs_close(struct file* file) {
  // 1. release the file descriptor
}
int vfs_write(struct file* file, const void* buf, int len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, int len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}
*/