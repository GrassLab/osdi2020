#include "vfs.h"
#include "mm.h"
#include "tmpfs.h"


// register the file system to the kernel.
// you can also initialize memory pool of the file system here.
int register_filesystem(filesystem_t *fs)
{
    if(_strcmp(fs->name,"tmpfs")==0){
        // init vops and fops
        tmpfs_v_ops = (vnode_operations_t*)kmalloc(sizeof(vnode_operations_t));
        tmpfs_f_ops = (file_operations_t*)kmalloc(sizeof(file_operations_t));

        tmpfs_v_ops->lookup = tmpfs_lookup;
        tmpfs_v_ops->create = tmpfs_create;
        tmpfs_f_ops->write = tmpfs_write;
        tmpfs_f_ops->read = tmpfs_read;

        return 0;
    }
    return -1;
}

// setting file system for root fs
void rootfs_init(){
    filesystem_t *fs = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    fs->name = "tmpfs";
    fs->setup_mount = tmpfs_setup_mount;

    register_filesystem(fs);

    // setup root file sysystem
    mount_t *mt = (mount_t*)kmalloc(sizeof(mount_t));
    fs->setup_mount(fs, mt);

    rootfs = mt;
}

void set_dentry(dentry_t *dentry, vnode_t *vnode, const char* dir_name){
    dentry->child_count = 0;
    dentry->child_dentry = (dentry_t*)kmalloc(sizeof(dentry_t)*MAX_CHILD_NUMBER);
    dentry->vnode = vnode; 
    strcpy(dentry->dname, dir_name);
}

file_t *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    if(flags == O_CREAT){ // create and open file
        vnode_t* target;
        rootfs->root->v_ops->create(rootfs->dentry,&target,pathname);

        file_t* fd = (file_t*)kmalloc(sizeof(file_t));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    }else{ // open file
        vnode_t* target;
        int ret = rootfs->root->v_ops->lookup(rootfs->dentry,&target,pathname);

        if(ret == -1){
            if (_strcmp(pathname,"/")!=0)
                printf("\n[vfs open] file not found!\n");
            return (file_t*)0; // NULL
        }
        printf("\n[vfs open] filename: %s\n", pathname);
        file_t* fd = (file_t*)kmalloc(sizeof(file_t));
        fd->vnode = target;
        fd->f_ops = target->f_ops;
        fd->f_pos = 0;
        return fd;
    }
}
int vfs_close(file_t *file)
{
    // 1. release the file descriptor
    printf("\n[vfs close] Close file at %d\n", file);
	kfree((unsigned long)file);
	return 0;
}
int vfs_write(file_t *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    int x = file->f_ops->write(file,buf,len);
    if(x == -1){
        printf("\n[vfs write] Exceed max length of file\n");
    }
	return x;
}
int vfs_read(file_t *file, void *buf, size_t len)
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file,buf,len);
}