#include "tmpfs.h"
#include "str.h"
#include "uart.h"
#include "buddy.h"
struct mount* rootfs;
struct filesystem *all_file_sys[NUM_OF_FILESYS];
int tmpfs_vnode_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name);
int tmpfs_vnode_creat(struct vnode* dir_node, struct vnode** target, const char* component_name);
struct vnode *user_default_vnode;




void set_init_vnode(struct vnode *root_vnode, struct mount *mount)
{
  root_vnode->v_ops = (struct vnode_operations*)varied_allocate(sizeof(struct vnode_operations));
  root_vnode->f_ops = (struct file_operations*)varied_allocate(sizeof(struct file_operations));
  root_vnode->mount = mount;
  root_vnode->num_of_child = -1;
  for(int i=0;i<DENTRY_MAX_CHILD;i++)
  {
    root_vnode->child[i] = 0;
  }
  root_vnode->v_ops->lookup = tmpfs_vnode_lookup;
  root_vnode->v_ops->create = tmpfs_vnode_creat;
  /*root_vnode->f_ops->write = //TO_DO
  root_vnode->f_ops->read =
  */
}

int tmpfs_mount_setup(struct filesystem* fs, struct mount* mount)
{
  mount->fs = fs;

  struct vnode *root_vnode = (struct vnode*)varied_allocate(sizeof(struct vnode));
  if(root_vnode == 0){
    uart_puts("tmpfs_mount_setup allocate mount struct faild");
    return 0;
  }
  set_init_vnode(root_vnode, mount);
  root_vnode->num_of_child = 0;
  mount->root = root_vnode;
  return 1;
}

int register_filesystem(struct filesystem* fs) {
  // register the file system to the kernel.
  if(my_strcmp(fs->name, "tmpfs", 3) == 0)
  {
    fs->setup_mount = tmpfs_mount_setup;
    rootfs = (struct mount*)varied_allocate(sizeof(struct mount));
    if (fs->setup_mount(fs, rootfs)) return 1;
  }
  return 0;
  // you can also initialize memory pool of the file system here.
}

int found_last_dir_vnode(const char* pathname, struct vnode **found)
{
  struct vnode *vnow= rootfs->root;
  if(pathname[0]!='/')
  {
    vnow = user_default_vnode;
  }

  int path_len = my_strlen( pathname );
  int now_pos = 0, pstart, pend, last_dir_len=0;

  for(int i=path_len-1;i>=0;i--)
  {
    if(pathname[i] == '/')
    {
      last_dir_len=i;
      break;
    }
  }
  
  if(last_dir_len == 0)
  {
    *found = vnow;
    return 1;
  }

  if(pathname[0]=='/') now_pos=1;

  while(1)
  {
    pstart = now_pos;
    pend = 0;
    for(int i = now_pos; i<=last_dir_len; i++)
    {
      if(pathname[i] == '/')
      {
        pend = i;
        break;
      }
    }

    if(pend != 0)
    {
      for(int i=0; i < vnow->num_of_child; i++)
      {
        if( my_strcmp(vnow->child[i]->name, (pathname+pstart), (pend-pstart)) == 0)
        {
          vnow = vnow->child[i]->vnode;
          now_pos = pend + 1;
          break;
        }
      }
      if(now_pos == (last_dir_len+1))
      {
        *found = vnow;
        return 1;
      }
    }
    else
    {
      return 0;
    }
  }
  return 0;
}

struct file* vfs_open(const char* pathname, int flags) {
  // 1. Lookup pathname from the root vnode.
  // 2. Create a new file descriptor for this vnode if found.
  // 3. Create a new file if O_CREAT is specified in flags.
  struct vnode *parent_dir=0, *thisfile=0; //dir vnode
  char *filename=0;
  int path_name_len = my_strlen(pathname);
  struct file* myopenfile=0;
  int result = found_last_dir_vnode(pathname, &parent_dir); //preprocess pathname to found last dir vnode
  if(result == 0 || parent_dir == 0)
  {
    uart_puts("vfs_open fail to find path!");
    return 0;
  }

  for(int i=path_name_len-1;i>=0;i--)
  {
    if(pathname[i] == '/')
    {
      filename = (char*)varied_allocate(path_name_len-i);
      my_strcpy(filename, (pathname+i+1), my_strlen((pathname+i+1)));
    }
  }
  if(filename == 0)
  {
    filename = (char*)varied_allocate(path_name_len);
    my_strcpy(filename, pathname, my_strlen(pathname));
  }

  int state = 0;
  state = parent_dir->v_ops->lookup(parent_dir, &thisfile, filename);
  //if(parent_dir == rootfs->root) uart_puts("root as default\r\n");
  if(state == 0 && flags == O_CREAT)
  {
    uart_puts("vfs_open create file start\r\n");
    state = 0;
    state = parent_dir->v_ops->create(parent_dir, &thisfile, filename);
    if(state == 0)
    {
      varied_free(filename);
      uart_puts("vfs_open create file faild\r\n");
      return 0;
    }
  }
  else if(state != 0 && flags == O_CREAT)
  {
    varied_free(filename);
    uart_puts("vfs_open create file exist\r\n");
    return 0;
  }
  else if(state == 0 && flags!=O_CREAT)
  {
    varied_free(filename);
    uart_puts("vfs_open file not exist and not CREAT flag\r\n");
    return 0;
  }
  
  myopenfile = (struct file*)varied_allocate(sizeof(struct file));
  if(myopenfile == 0)
  {
    uart_puts("vfs_open file allocate faild\r\n");
    return 0;
  }
  myopenfile->vnode = thisfile;
  myopenfile->f_pos = 0;
  myopenfile->flags = 0;
  myopenfile->f_ops = 0;
  myopenfile->f_ops = (struct file_operations*)varied_allocate(sizeof(struct file_operations));
  if(myopenfile->f_ops == 0)
  {
    uart_puts("vfs_open file fops allocate faild\r\n");
    return 0;
  }
  myopenfile->f_ops->read = parent_dir->f_ops->read;
  myopenfile->f_ops->write = parent_dir->f_ops->write;
  if(varied_free(filename)) return myopenfile;
  uart_puts("vfs_open filename free faild\r\n");
  return 0;
}
int vfs_close(struct file* file) {
  // 1. release the file descriptor
  if(varied_free(file)) return 1;
  uart_puts("vfs_close faild\r\n");
  return 0;
}
int vfs_write(struct file* file, const void* buf, int len) {
  // 1. write len byte from buf to the opened file.
  // 2. return written size or error code if an error occurs.
}
int vfs_read(struct file* file, void* buf, int len) {
  // 1. read min(len, readable file data size) byte to buf from the opened file.
  // 2. return read size or error code if an error occurs.
}

void filesystem_init()
{
    for(int i=0;i<NUM_OF_FILESYS;i++)
    {
        all_file_sys[i] = 0;
    }
}

void set_init_dentry(struct dentry *dentry, struct vnode* vnode, const char* str)
{
  dentry->name = (char*)varied_allocate(my_strlen(str)+1);
  my_strcpy(dentry->name , str, my_strlen(str));
  dentry->vnode = vnode;
  dentry->parent = 0;
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

void rootfs_init() {
  filesystem_init();
  struct filesystem *root_file_sys = (struct filesystem*)varied_allocate(sizeof(struct filesystem));
  root_file_sys->name = "tmpfs";
  register_filesystem(root_file_sys);
  rootfs = (struct mount*)varied_allocate(sizeof(struct mount));
  root_file_sys->setup_mount(root_file_sys, rootfs);
  struct dentry *root_dentry = (struct dentry*)varied_allocate(sizeof(struct dentry));
  set_init_dentry(root_dentry, rootfs->root, "/");

  user_default_vnode = rootfs->root;
}