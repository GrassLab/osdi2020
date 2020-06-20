#include "vfs.h"
#include "tmpfs.h"
#include "mm.h"

filesystem_t* fs;

int tmpfs_setup_mount (filesystem_t * fs, mount_t * mount );
int tmpfs_lookup (dentry_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_create (dentry_t * dir_node, vnode_t ** target, const char * component_name );
int tmpfs_write (file_t * file, const void * buf, size_t len );
int tmpfs_read (file_t * file, void * buf, size_t len );
void list_tmpfs(dentry_t* dir);

void set_tmpfs_vnode(vnode_t * vnode){ 
	vnode->v_ops = tmpfs_v_ops;
	vnode->f_ops = tmpfs_f_ops;
}

int tmpfs_setup_mount(filesystem_t * fs, mount_t * mt)
{	
    vnode_t *vnode = (vnode_t*)kmalloc(sizeof(vnode_t));	
	set_tmpfs_vnode(vnode);

	dentry_t *dentry = (dentry_t*)kmalloc(sizeof(dentry_t));
	set_dentry(dentry, vnode, "/");
   
	mt->fs = fs;
    mt->root = vnode;
    mt->dentry = dentry;

    return 0;
}

int tmpfs_lookup(dentry_t * dir_node, vnode_t ** target, const char * component_name)
{
    for(int i = 0; i < dir_node->child_count; i++){
		if(_strcmp(dir_node->child_dentry[i].dname, component_name) == 0){
			*target = dir_node->child_dentry[i].vnode;
			return 0;
		}
	}

	return -1;
}

int tmpfs_create(dentry_t * dir_node, vnode_t ** target, const char * component_name)
{
    printf("------------------create file ------------------\n");

	int res = tmpfs_lookup(dir_node, target, component_name);
	if(res != -1){
		printf("\n[create file] file '%s' already exist.\n", component_name);
		return 0;
	}

  	vnode_t *vnode = (vnode_t*)kmalloc(sizeof(vnode_t));
	set_tmpfs_vnode(vnode); 
	vnode->internal = (void *)kmalloc(sizeof(tmpfs_node_t));	

	dentry_t* child = (dentry_t*)kmalloc(sizeof(dentry_t));

	set_dentry(child, vnode, component_name);

	if(dir_node->child_count<MAX_CHILD_NUMBER){
		dir_node->child_dentry[dir_node->child_count++] = *child;
	}else{
		printf("NOT HANDLE THIS RIGHT NOW!\r\n");
		while(1);
	}

	printf("\n[create file] %s\r\n", dir_node->child_dentry[(dir_node->child_count)-1].dname);

	*target = vnode;
	return 0;
}

int tmpfs_write(file_t * file, const void * buf, size_t len)
{
    if((file->f_pos) + len > TMPFS_FILE_SIZE){
		return -1;
	}

    vnode_t* vnode = file->vnode;

    char *buffer = (char *)buf;
    tmpfs_node_t *file_node = (tmpfs_node_t *)vnode->internal;
    char *file_text = file_node->buffer;
    unsigned int i = 0;

    for(; i < len; i++){
        file_text[file->f_pos++] = buffer[i];
    }

    vnode->v_size = len;
    file_text[i] = EOF;
    return i;
}

int tmpfs_read(file_t * file, void * buf, size_t len)
{
    vnode_t* vnode = file->vnode;

    tmpfs_node_t *file_node = (tmpfs_node_t *)vnode->internal;
  	char *file_text = file_node->buffer;
	char *buffer = (char *)buf;
	unsigned int i = 0;	
	for(; i < len; i++){
		if(i < vnode->v_size){ 
            buffer[i] = file_text[i];
	    }else{
			break;
		}
	}

	return i;
} 

void list_tmpfs(dentry_t* dir){
	printf("\n[list file] dir: %s\n", dir->dname);
	for(int i = 0; i < dir->child_count; i++){
		printf("File %d: '%s' \n", i ,dir->child_dentry[i].dname);
	}
	return;
}