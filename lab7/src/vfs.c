#include "uart.h"
#include "string.h"
#include "buddy.h"
#include "tmpfs.h"
#include "vfs.h"

static struct filesystem *file_systems;

void set_dentry(struct dentry *dentry,struct vnode* vnode, const char* str){
     dentry->child_count = 0;
     dentry->vnode = vnode;
     strcpy(dentry->dname, str);
}

void init_rootfs(){
    
    // set file system for rootfs
    struct filesystem *fs = (struct filesystem*)kmalloc(sizeof(struct filesystem));
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount_tmpfs;
    
    register_filesystem(fs);

    struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    set_tmpfs_vnode(vnode);

    struct tmpfs_node* tmpfs_node = (struct tmpfs_node*)kmalloc(sizeof(struct tmpfs_node));
    tmpfs_node->flag = ROOT_DIR;
    vnode->internal = (void *)tmpfs_node;
    
    struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
    set_dentry(dentry, vnode, "/");
    dentry->flag = ROOT_DIR;
    
    // create a struct mount object and let rootfs point to it
    struct mount *mnt = (struct mount*)kmalloc(sizeof(struct mount));
    mnt->fs= fs; 
    mnt->root = vnode;
    mnt->dentry = dentry;
    rootfs= mnt; 

    //set current working directory 
    current_dent = dentry;
    uart_puts("[Init_rootfs]\r\n");
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
    if(strcmp(fs->name,"tmpfs") == 0){
        tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
        tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));

        tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;
        tmpfs_v_ops->ls = ls_tmpfs;
        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs;
        return 0;
    }
    return -1;
}


struct file* vfs_open(const char* pathname, int flags) {
    struct vnode* target;
    // 1. Lookup pathname from the root vnode.
    int ret = rootfs->root->v_ops->lookup(rootfs->dentry, &target, pathname);
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    if(flags == O_CREAT){
        if(ret == -1){
            rootfs->root->v_ops->create(rootfs->dentry, &target, pathname);   
        }else{
            uart_puts("FILE ALREADY EXIST!\r\n");
        }   
    }else{
        if(ret == -1){
            if(strcmp(pathname, "/") == 0){
                rootfs->root->v_ops->ls(current_dent);
                return (struct file*)NULL;
            }else{
                uart_puts("FILE NOT FOUND!\r\n");
                return (struct file*)NULL;
            }
        }   
    }
    struct file* fd = (struct file*)kmalloc(sizeof(struct file));
    fd->vnode = target;
    fd->f_ops = target->f_ops;
    fd->f_pos = 0;
    return fd; 
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
