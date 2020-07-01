#include "vfs.h"
#include "mm.h"
#include "tmpfs.h"
#include "sd.h"
#include "fat32.h"

// register the file system to the kernel.
// you can also initialize memory pool of the file system here.
int register_filesystem(filesystem_t *fs)
{
    // if(_strcmp(fs->name,"tmpfs") == 0){
    //     // init vops and fops
    //     tmpfs_v_ops = (vnode_operations_t*)kmalloc(sizeof(vnode_operations_t));
    //     tmpfs_f_ops = (file_operations_t*)kmalloc(sizeof(file_operations_t));

    //     tmpfs_v_ops->lookup = tmpfs_lookup;
    //     tmpfs_v_ops->create = tmpfs_create;
    //     tmpfs_f_ops->write = tmpfs_write;
    //     tmpfs_f_ops->read = tmpfs_read;

    //     return 0;
    // }
    return -1;
}

// setting file system for root fs
void rootfs_init(){
    sd_init();
    fat32_init();

    // filesystem_t *fs = (filesystem_t*)kmalloc(sizeof(filesystem_t));
    // fs->name = "tmpfs";
    // fs->setup_mount = tmpfs_setup_mount;

    // setup root file sysystem
    mount_t *mt = (mount_t*)kmalloc(sizeof(mount_t));
    fat32_fs->setup_mount(fat32_fs, mt);
    rootfs = mt;
    fat32_ls(mt->root);

}

void set_dentry(dentry_t *dentry, vnode_t *vnode, const char* dir_name){
    dentry->child_count = 0;
    dentry->child_dentry = (dentry_t*)kmalloc(sizeof(dentry_t)*MAX_CHILD_NUMBER);
    dentry->vnode = vnode; 
    strcpy(dentry->dname, dir_name);
}


vnode_t *vnode_create(mount_t *mount, vnode_operations_t *v_ops, file_operations_t *f_ops)
{
    vnode_t *tmp_vnode;
    tmp_vnode = kmalloc(sizeof(vnode_t));
    if(tmp_vnode == NULL)
        return NULL;
    tmp_vnode->mount = mount;
    tmp_vnode->v_ops = v_ops;
    tmp_vnode->f_ops = f_ops;
    return tmp_vnode;
}

file_t* create_file(vnode_t* target, int flags){
    file_t* fd = (file_t*)kmalloc(sizeof(file_t));
    // fd->vnode = target;
    fd->vnode = target;
    fd->f_ops = target->f_ops;
    fd->f_pos = 0;
    fd->flags = flags;
    return fd;
}

file_t *vfs_open(const char *pathname, int flags)
{
    // 1. Lookup pathname from the root vnode.
    // 2. Create a new file descriptor for this vnode if found.
    // 3. Create a new file if O_CREAT is specified in flags.
    
    // create and open file
    vnode_t* target;
    if(flags == O_CREAT){ 
        rootfs->root->v_ops->create(rootfs->root, &target, pathname);

        //create file struct
        return create_file(target, flags);
    }else{ // open file
        int ret = rootfs->root->v_ops->lookup(rootfs->root, &target, pathname);

        if(ret == -1){
            if (_strcmp(pathname, "/") != 0)
                printf("\n[vfs open] file cannot found!\n");
            return (file_t*)0; // NULL
        }
        printf("\n[vfs open] filename is: %s\n", pathname);
        return create_file(target, flags);
    }
}
int vfs_close(file_t *file)
{
    // 1. release the file descriptor
	kfree((unsigned long)file);
	return 0;
}

int vfs_write(file_t *file, const void *buf, size_t len)
{
    // 1. write len byte from buf to the opened file.
    // 2. return written size or error code if an error occurs.
    int x = file->f_ops->write(file, buf, len);
    if(x == -1){
        printf("\n[vfs write] Exceed max length of file\n");
    }
	return x;
}

int vfs_read(file_t *file, void *buf, size_t len)
{
    // 1. read min(len, readable file data size) byte to buf from the opened file.
    // 2. return read size or error code if an error occurs.
    return file->f_ops->read(file, buf, len);
}