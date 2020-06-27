#include "tmpfs.h"


#include "mm.h"
#include "vfs.h"
#include "lib/string.h"
// #include "printf.h"


struct vnode_operations tmpfs_v_ops = {lookup_tmpfs, 0};
struct file_operations tmpfs_f_ops;


void setup_fs_tmpfs(struct filesystem *fs)
{
    fs->name = "tmpfs";
    fs->setup_mount = setup_mount_tmpfs;
}

struct vnode *create_tmpfs_vnode()
{
    struct vnode *vnode = kmalloc(sizeof(struct vnode));
    vnode->mount = 0;//NULL
    vnode->v_ops = &tmpfs_v_ops;
    vnode->f_ops = &tmpfs_f_ops;
    vnode->cache = 0;//NULL
    return vnode;
}

int setup_mount_tmpfs(struct filesystem *fs, struct mount *mnt)
{
    struct vnode *vnode = create_tmpfs_vnode();
    vnode ->type = VNODE_TYPE_DIR;
    mnt->fs= fs;
    mnt->root = vnode;

    return 0;
}

int lookup_tmpfs(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    vnode->cache = kmalloc(sizeof(struct vnode_cache));
    memset(vnode->cache, 0U, sizeof(struct vnode_cache));
    return 0;
}

// void list_tmpfs(struct dentry *dir){
//     printf("\n[list file] dir: %s\n", dir->dname);
//     for(int i=0; i<dir->child_count; i++){
//         printf("File %d: '%s' \r\n",i,dir->child_dentry[i].dname);
//     }
//     return;
// }

//     return -1;
// }

// int create_tmpfs(struct dentry *dir, struct vnode **target, const char *component_name){
//     printf("================= create file =================\n");

//     int res = lookup_tmpfs(dir,target,component_name);
//     if(res != -1){
//         printf("\n[create file] file '%s' already exist.\n", component_name);
//         return 0;
//     }

//       struct vnode *vnode = kmalloc(sizeof(struct vnode));
//     set_tmpfs_vnode(vnode); 
//     vnode->internal = kmalloc(sizeof(struct tmpfs_node));

//     struct dentry *child = kmalloc(sizeof(struct dentry));

//     set_dentry(child,vnode,component_name);

//     if(dir->child_count<MAX_CHILD){
//         dir->child_dentry[dir->child_count++] = *child;
//     }else{
//         printf("NOT HANDLE THIS RIGHt NOW!\r\n");
//         while(1);
//     }

//     printf("\n[create file] %s\r\n", dir->child_dentry[(dir->child_count)-1].dname);

//     *target = vnode;
//     return 0;
// }

// int write_tmpfs(struct file *file, const void *buf, unsigned len){
//     if((file->f_pos)+len > TMPFS_FILE_SIZE){
//         return -1;
//     }

//     struct vnode *vnode = file->vnode;

//     char *buffer = (char *)buf;
//     struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
//     char *file_text = file_node->buffer;
//     unsigned int i=0;

//     for(; i<len; i++){
//         file_text[file->f_pos++] = buffer[i];
//     }

//     file_text[i] = EOF;
//     return i;
// }

// int read_tmpfs(struct file *file, void *buf, unsigned len){
//     struct vnode *vnode = file->vnode;

//     struct tmpfs_node *file_node = (struct tmpfs_node *)vnode->internal;
//       char *file_text = file_node->buffer;
//     char *buffer = (char *)buf;
//     unsigned int i=0;
//     for(; i<len; i++){
//         if(file_text[i] != (unsigned char)(EOF)){ 
//             buffer[i] = file_text[i];
//         }else{
//             break;
//         }
//     }

//     return i;
// }