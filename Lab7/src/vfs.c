#include "include/vfs.h"
#include "include/string.h"
#include "include/mm.h"
#include "include/uart.h"
#include "include/kernel.h"
#include "include/fs/tmpfs.h"

void set_dentry(struct dentry *dentry,struct vnode* vnode,\
                 const char* str){
         
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
}


int register_filesystem(struct filesystem* fs) {
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

  	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));	
	set_tmpfs_vnode(vnode);

  	struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
	set_dentry(dentry,vnode,"/");

  	// setup root file sysystem
  	struct mount *mt = (struct mount*)kmalloc(sizeof(struct mount));
  	mt->fs= fs;
  	mt->root = vnode;
	mt->dentry = dentry;

  	rootfs= mt; 
  	return 0;
  }
  return -1;
}


struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  if(flags == O_CREAT){
	struct vnode* target;
	int ret = rootfs->root->v_ops->lookup(rootfs->dentry,&target,pathname); 
  	if(ret == -1){
		rootfs->root->v_ops->create(rootfs->dentry,&target,pathname);	
	}
	struct file* fd = (struct file*)kmalloc(sizeof(struct file));
	fd->vnode = target;
	fd->f_ops = target->f_ops;
	fd->f_pos = 0;
	return fd;
  }
  else{
	struct vnode* target;
	int ret = rootfs->root->v_ops->lookup(rootfs->dentry,&target,pathname); 
        
	if(ret == -1){
		if (strcmp(pathname,"/")!=0)
			printf("### FILE NOT FOUND!\r\n");
  		return (struct file*)NULL;
	}
	printf("### FOUND FILE %s\r\n",pathname);
	struct file* fd = (struct file*)kmalloc(sizeof(struct file));
	fd->vnode = target;
	fd->f_ops = target->f_ops;
	fd->f_pos = 0;
	return fd;
  }
  
}

int vfs_close(struct file* file){
	printf("### Close file at %x\r\n",file);
	kfree((unsigned long)file);
	return 0;
}

int vfs_write(struct file* file, const void* buf, size_t len){
	return file->f_ops->write(file,buf,len);
}

int vfs_read(struct file* file, void* buf, size_t len){
	return file->f_ops->read(file,buf,len);
}
