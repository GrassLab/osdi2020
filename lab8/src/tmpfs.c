#include "uart.h"
#include "string.h"
#include "buddy.h"
#include "vfs.h"
#include "tmpfs.h"
void set_tmpfs_vnode(struct vnode* vnode){ 
    // create root directory's vnode
    vnode->v_ops = tmpfs_v_ops;
    vnode->f_ops = tmpfs_f_ops;
}

int setup_mount_tmpfs(struct filesystem* fs, struct mount* mnt){
    struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    set_tmpfs_vnode(vnode);

    struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
    set_dentry(dentry, vnode, "/");
    dentry->flag = ROOT_DIR;
 
    mnt->fs= fs; 
    mnt->root = vnode;
    mnt->dentry = dentry;
    
    //set current working directory 
    current_dent = dentry;
    return 0;
}

int ls_tmpfs(struct dentry* dir){
    for(int i = 0; i < dir->child_count; i++){
        uart_puts(dir->child_dentry[i]->dname);       
        uart_puts("\r\n");
    }   
    return 0; 
}

int lookup_tmpfs(struct dentry* dir, struct vnode** target, const char* component_name){
    for(int i = 0; i < dir->child_count; i++){
        if(strcmp(dir->child_dentry[i]->dname, component_name) == 0){
            *target = dir->child_dentry[i]->vnode;
            return 0;
        }
    }   
    return -1; 
}

int create_tmpfs(struct dentry* dir, struct vnode** target, const char* component_name){
    //create a regular file vnode
    struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    set_tmpfs_vnode(vnode);

    struct tmpfs_node* tmpfs_node = (struct tmpfs_node*)kmalloc(sizeof(struct tmpfs_node));
    tmpfs_node->flag = REGULAR_FILE;
    vnode->internal = (void *)tmpfs_node;
    //create new edge (new vnode connect to other in future)
    struct dentry* child = (struct dentry*)kmalloc(sizeof(struct dentry));
    set_dentry(child, vnode, component_name);
    child->parent_dentry = dir;
    child->flag = REGULAR_FILE;

    if(dir->child_count < MAX_CHILD)
        dir->child_dentry[dir->child_count++] = child;
    else{
        uart_puts("Exceed CHILD NUMBER!");
        uart_puts("\r\n");
        while(1);
    }
    
    *target = vnode;

    uart_puts("Create file : ");
    uart_puts(dir->child_dentry[dir->child_count-1]->dname);
    uart_puts("\r\n");
    return 0;
}

int write_tmpfs(struct file* file, const void* buf, size_t len){
    struct vnode* vnode = file->vnode;

    struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;

    if(file_node->flag != REGULAR_FILE){
        uart_puts("ERROR! TRY TO WRITE SOMETHING NOT A FILE");
        uart_puts("\r\n");
        return -1;
    }

    char *file_text = file_node->buffer;
    char *buffer = (char *)buf;
    unsigned long i = 0;

    for(; i < len; i++){
        file_text[file->f_pos + i] = buffer[i];
    }
    file->f_pos += i;
    file_text[file->f_pos] = EOF;
    return i;
}

int read_tmpfs(struct file* file, void* buf, size_t len){
    struct vnode* vnode = file->vnode;

    struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;

    if(file_node->flag != REGULAR_FILE){
        uart_puts("ERROR! TRY TO WRITE SOMETHING NOT A FILE");
        uart_puts("\r\n");
        return -1;
    }

    char *file_text = file_node->buffer;
    char *buffer = (char *)buf;
    unsigned int i = 0;
    for(; i < len; i++){
        if(file_text[file->f_pos + i] != (unsigned char)(EOF)){
            buffer[i]  = file_text[file->f_pos + i];
        }else {
            break;
        }
    }
    file->f_pos += i;
    return i;
}

