#include "vfs.h"
#include "mm.h"
#include "tmpfs.h"
#include "printf.h"
#include "lib/string.h"

void set_dentry(struct dentry *dentry,struct vnode* vnode, const char* str){
    dentry->child_count = 0;
    dentry->child_dentry = (struct dentry*)kmalloc(sizeof(struct dentry)*MAX_CHILD);
    dentry->vnode = vnode; 
    strcpy(dentry->dname , str);
}


void rootfs_init(){
    // setting file system for root fs
    struct filesystem *fs = (struct filesystem*)kmalloc(sizeof(struct filesystem));
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount_tmpfs;

    register_filesystem(fs);

    // setup root file sysystem
    struct mount *mt = (struct mount*)kmalloc(sizeof(struct mount));
    fs->setup_mount(fs,mt);

    rootfs = mt;
}


int register_filesystem(struct filesystem* fs){
    // register the file system to the kernel.
    // you can also initialize memory pool of the file system here.
    if(strcmp(fs->name,"tmpfs")==0){
        // init vops and fops
        tmpfs_v_ops = (struct vnode_operations*)kmalloc(sizeof(struct vnode_operations));
        tmpfs_f_ops = (struct file_operations*)kmalloc(sizeof(struct file_operations));

        tmpfs_v_ops->lookup = lookup_tmpfs;
        tmpfs_v_ops->create = create_tmpfs;
        tmpfs_f_ops->write = write_tmpfs;
        tmpfs_f_ops->read = read_tmpfs;

        return 0;
    }
    return -1;
}


struct file* vfs_open(const char* pathname, int flags) {
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    if(flags == O_CREAT){ // create and open file
        struct vnode* target;
        rootfs->root->v_ops->create(rootfs->dentry,&target,pathname);

        struct file* fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    }else{ // open file
        struct vnode* target;
        int ret = rootfs->root->v_ops->lookup(rootfs->dentry,&target,pathname);

        if(ret == -1){
            if (strcmp(pathname,"/")!=0)
                printf("\n[vfs open] file not found!\n");
            return (struct file*)0; // NULL
        }
        printf("\n[vfs open] filename: %s\n", pathname);
        struct file* fd = (struct file*)kmalloc(sizeof(struct file));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    }
}

int vfs_close(struct file* file){
	printf("\n[vfs close] Close file at %d\n", file);
	kfree((void *)file);
	return 0;
}

int vfs_write(struct file* file, const void* buf, unsigned len){
    int x = file->f_ops->write(file,buf,len);
    if(x == -1){
        printf("\n[vfs write] Exceed max length of file\n");
    }
	return x;
}

int vfs_read(struct file* file, void* buf, unsigned len){
	return file->f_ops->read(file,buf,len);
} 