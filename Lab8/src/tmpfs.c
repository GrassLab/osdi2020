#include "include/vfs.h"
#include "include/mm.h"
#include "include/uart.h"
#include "include/fs/tmpfs.h"


void set_tmpfs_vnode(struct vnode* vnode){ 
	 // create root directory's vnode
         vnode->v_ops = tmpfs_v_ops;
         vnode->f_ops = tmpfs_f_ops;
}

int setup_mount_tmpfs(struct filesystem* fs, struct mount* mt){	
	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
 	set_tmpfs_vnode(vnode);

	struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
 	set_dentry(dentry,vnode,"/");
 	dentry->flag = ROOT_DIR;

	mt->fs= fs;
	mt->root = vnode;
	mt->dentry = dentry;
	
	return 0;
}

void ls_tmpfs(struct dentry* dir){

	printf("=== Lookup file in '%s' ===\r\n",dir->dname);	
	for(int i=0;i<dir->child_count;i++){
		printf("-> File %d: '%s' \r\n",i,dir->child_dentry[i]->dname);
	}
	printf("=== end ===\r\n");
}

int lookup_tmpfs(struct dentry* dir, struct vnode** target, \
		const char* component_name){

	for(int i=0;i<dir->child_count;i++){
		if(strcmp(dir->child_dentry[i]->dname, component_name)==0){
			*target = dir->child_dentry[i]->vnode;
			return 0;
		}
	}

	return -1;
}

int create_tmpfs(struct dentry* dir, struct vnode** target, \
		const char* component_name){

  	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
	set_tmpfs_vnode(vnode); 

	struct tmpfs_node* tmpfs_node = (struct tmpfs_node*)kmalloc(sizeof(struct tmpfs_node));
        tmpfs_node->flag = REGULAR_FILE;
	vnode->internal = (void *)tmpfs_node;	

	struct dentry* child = (struct dentry*)kmalloc(sizeof(struct dentry));	
	set_dentry(child,vnode,component_name);
	child->parent_dentry = dir;
	child->flag = REGULAR_FILE;

	if(dir->child_count<MAX_CHILD)
		dir->child_dentry[dir->child_count++] = child;
	else{
		printf("NOT HANDLE THIS RIGHt NOW!\r\n");
		while(1);
	}

	printf("### Create file %s\r\n",dir->child_dentry[dir->child_count-1]->dname);
	
	*target = vnode;
	return 0;
}

int mkdir_tmpfs(struct dentry* dir, struct vnode** target, const char *component_name){
	
  	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
	set_tmpfs_vnode(vnode); 

	struct tmpfs_node* tmpfs_node = (struct tmpfs_node*)kmalloc(sizeof(struct tmpfs_node));
        tmpfs_node->flag = DIRECTORY;
	vnode->internal = (void *)tmpfs_node;	

	struct dentry* child = (struct dentry*)kmalloc(sizeof(struct dentry));	
	set_dentry(child,vnode,component_name);
	child->parent_dentry = dir;
	child->flag = DIRECTORY;
	
	if(dir->child_count<MAX_CHILD)
		dir->child_dentry[dir->child_count++] = child;
	else{
		printf("NOT HANDLE THIS RIGHt NOW!\r\n");
		while(1);
	}

	printf("### Create directory %s\r\n",dir->child_dentry[dir->child_count-1]->dname);
	
	*target = vnode;
	return 0;
}

int write_tmpfs(struct file* file, const void* buf, size_t len){
	  struct vnode* vnode = file->vnode;
	   
          char *buffer = (char *)buf;
          struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
  	  
	  if(file_node->flag != REGULAR_FILE){
	  	  printf("ERROR! TRY TO WRITE SOMETHING NOT A FILE\r\n");
		  return -1;
	  }

	  char	*file_text = file_node->buffer;
          unsigned int i=0;
  
          for(;i<len;i++){
                  file_text[file->f_pos+i] = buffer[i];
          }
   	  
	  file_text[i] = EOF;
          return i;
}

int read_tmpfs(struct file* file, void* buf, size_t len){
	struct vnode* vnode = file->vnode;

        struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
  	char *file_text = file_node->buffer;
	
	if(file_node->flag != REGULAR_FILE){
		printf("ERROR! TRY TO WRITE SOMETHING NOT A FILE\r\n");
		return -1;
	}

	char *buffer = (char *)buf;
	unsigned int i=0;	
	for(;i<len;i++){
		if(file_text[i] != (unsigned char)(EOF)) 
                    	buffer[i]  = file_text[i];
	        else
			break;	
	}
	
	return i;

}

// For tmpfs, missing dentry means actually file not exist 
int load_dent_tmpfs(struct dentry *dent,char *component_name){
	// just used to make compiler happy
	printf("%d %s\r\n",dent,component_name);
	return -1;
}
