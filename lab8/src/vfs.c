#include "uart.h"
#include "string.h"
#include "buddy.h"
#include "fat32.h"
#include "tmpfs.h"
#include "vfs.h"

static struct filesystem *file_systems;

void set_dentry(struct dentry *dentry,struct vnode* vnode, const char* str){
     dentry->child_count = 0;
     dentry->vnode = vnode;
     strcpy(dentry->dname, str);
}

void init_rootfs(){
    sd_init();
    int ret = fat32_partition();
    if(ret == 0){
        uart_puts("fat_getpartition FAIL\r\n");
    }
    
    // set file system for rootfs
    struct filesystem *fs = (struct filesystem*)kmalloc(sizeof(struct filesystem));
    fs->name = "fat32";
    fs->setup_mount = setup_mount_fat32;
    register_filesystem(fs);
    
    // create a struct mount object and let rootfs point to it
    struct mount *mnt = (struct mount*)kmalloc(sizeof(struct mount));
    fs->setup_mount(fs, mnt);
    rootfs= mnt; 
    current_dent = mnt->dentry;
    

    uart_puts("[Init_rootfs]\r\n");
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel and initialize memory pool of the file system here.
    if(strcmp(fs->name,"tmpfs") == 0){
        tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
        tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));

        tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;
        tmpfs_v_ops->ls = ls_tmpfs;
        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs;
        return 0;
    }else if(strcmp(fs->name,"fat32") == 0){
        fat32_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
        fat32_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));

        fat32_v_ops->lookup = lookup_fat32;
        fat32_v_ops->load_dent = load_dent_fat32;
        fat32_f_ops->write = write_fat32;
        fat32_f_ops->read = read_fat32;
        return 0;
    }
    return -1;
}


struct file* vfs_open(const char* pathname, int flags) {
    struct vnode* target;
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    struct dentry *dent = current_dent;
    if(flags == O_CREAT){
        int ret = rootfs->root->v_ops->lookup(rootfs->dentry, &target, pathname);
        if(ret == -1){
            rootfs->root->v_ops->create(rootfs->dentry, &target, pathname);   
        }else{
            uart_puts("FILE ALREADY EXIST!\r\n");
        }   
        struct file* fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        // store these info for easy access related info
        struct fat32_node *tmp = (struct fat32_node *)dent->vnode->internal;
        fd->parent_cluster = tmp->cluster;
        strcpy(fd->fname, pathname);
        return fd; 
    }else{
        int ret = -1;
        ret =  dent->vnode->v_ops->load_dent(dent, pathname);
        if(ret == -1){
            uart_puts("LOAD FAIL!\r\n");
            return (struct file*)NULL;;
        }else{
            uart_puts("LOAD SUCCESS!\r\n");
        }

        ret = rootfs->root->v_ops->lookup(rootfs->dentry, &target, pathname);
        if(ret == -1){
            if(strcmp(pathname, "/") == 0){
//                rootfs->root->v_ops->ls(current_dent);
                return (struct file*)NULL;
            }else{
                uart_puts("FILE NOT FOUND!\r\n");
                return (struct file*)NULL;
            }
        }   
        struct file* fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        
        struct fat32_node *tmp = (struct fat32_node *)dent->vnode->internal;
        fd->parent_cluster = tmp->cluster;
        strcpy(fd->fname, pathname);
        return fd; 
    }
}

int vfs_close(struct file* file) {
    // 1. release the file descriptor
    kfree((unsigned long)file);
    return 0;
}
int vfs_write(struct file* file, const void* buf, size_t len) {
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    return file->f_ops->write(file,buf,len);
}
int vfs_read(struct file* file, void* buf, size_t len) {
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file,buf,len);
}
