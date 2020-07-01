#include "tmpfs.h"
#include "str.h"
#include "uart.h"
#include "buddy.h"



int tmpfs_mount_setup(struct filesystem* fs, struct mount* mount)
{
  mount->fs = fs;

  struct vnode *root_vnode = (struct vnode*)varied_allocate(sizeof(struct vnode));
  //struct dentry *aaa = (struct dentry*)varied_allocate(sizeof(struct dentry));
  if(root_vnode == 0){
    uart_puts("tmpfs_mount_setup allocate mount struct faild");
    return 0;
  }
  set_init_vnode(root_vnode, mount);
  root_vnode->num_of_child = 0;
  mount->root = root_vnode;
  return 1;
}

int tmpfs_vnode_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
  for(int i=0;i<dir_node->num_of_child;i++)
  {
    if( my_strcmp(dir_node->child[i]->name, component_name, my_strlen(component_name)) == 0 )
    {
      *target = dir_node->child[i]->vnode;
      return 1;
    }
  }
  return 0;
}

int tmpfs_vnode_creat(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
  int index = dir_node->num_of_child;
  if(index+1 > DENTRY_MAX_CHILD)
  {
    uart_puts( "tmpfs_vnode_creat child filled.\n" );
    return 0;
  }
  dir_node->child[index] = (struct dentry*)varied_allocate(sizeof(struct dentry));
  if(dir_node->child[index] == 0)
  {
    uart_puts("tmpfs_vnode_creat allocate dentry faild\r\n");
    return 0;
  }

  *target = (struct vnode*)varied_allocate(sizeof(struct vnode));
  if(*target == 0)
  {
    uart_puts("tmpfs_vnode_creat allocate vnode faild\r\n");
    return 0;
  }
  
  set_init_vnode(*target, 0);
  dir_node->child[index]->vnode = *target;

  dir_node->child[index]->name = (char*)varied_allocate(my_strlen(component_name)+1);
  if(dir_node->child[index]->name == 0)
  {
    uart_puts("tmpfs_vnode_creat allocate vnode name faild\r\n");
    return 0;
  }
  //uart_puts("vnode create now\r\n");
  //my_printf("%d\r\n", my_strlen(component_name)+1);
  //while(1);
  my_strcpy(dir_node->child[index]->name , component_name, my_strlen(component_name));
  //my_printf("%s\r\n", dir_node->child[index]->name);
  dir_node->child[index]->parent = dir_node;
  dir_node->num_of_child+=1;
  return 1;
}

int tmpfs_write(struct file* file, const void* buf, int len)
{
  if(file->f_pos + len > BUFFER_MAX_LEN)
  {
    uart_puts("tmpfs_write: too many write bytes\r\n");
    return 0;
  }
  struct tmpfs_filenode* tmp_file_node = file->vnode->internal;
  if ( my_strcpy ((tmp_file_node->buffer + file->f_pos), (char*)buf, len) )
  {
    //my_printf("in tmpfs write %s\r\n", tmp_file_node->buffer + file->f_pos);
    file->f_pos += len;
    tmp_file_node->file_size += len;
    return 1;
  }
  uart_puts("tmpfs_write: strcpy faild\r\n");
  return 0;
}

int tmpfs_read(struct file* file, void* buf, int len)
{
  struct tmpfs_filenode* tmp_file_node = file->vnode->internal;
  if(len > tmp_file_node->file_size)
  {
    len = tmp_file_node->file_size;
    //uart_puts("tmpfs_read: too many read bytes\r\n");
    //return 0;
  }
  //my_printf("in tmpfs read %d : %s\r\n", len ,tmp_file_node->buffer);
  if ( my_strcpy ((char*)buf, (tmp_file_node->buffer), len) )
  {
    //my_printf("in tmpfs read 2222 %d : %s\r\n", len ,buf);
    return len;
  }
  uart_puts("tmpfs_read: strcpy faild\r\n");
  return 0;
}

