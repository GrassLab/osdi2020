#include "tmpfs.h"

struct file_operations* tmpfs_f_ops;
struct vnode_operations* tmpfs_v_ops;
int d_token, v_token, tb_token;

void init_dentry(struct dentry* dentry, struct vnode* vnode, const char* name) {
  dentry->vnode = vnode;
  strcpy(dentry->name, name);
}

int tmpfs_mount(struct filesystem* fs, struct mount* mount) {
  d_token = register_obj_allocator(sizeof(struct dentry), FIXED);
  tb_token = register_obj_allocator(sizeof(struct tmpfs_buf), FIXED);
  v_token = register_obj_allocator(sizeof(struct vnode), FIXED);
  struct dentry* dentry = (struct dentry*)fixed_obj_allocate(d_token);
  struct vnode* vnode = (struct vnode*)fixed_obj_allocate(v_token);
  struct vnode* root= (struct vnode*)fixed_obj_allocate(v_token);
  mount->fs = fs;
  vnode->v_ops = tmpfs_v_ops;
  vnode->f_ops = tmpfs_f_ops;

  dentry->type = DICTIONARY;
  dentry->vnode = vnode;

  init_dentry(dentry, vnode, "/");
  for (int i = 0; i < DIR_MAX; i++) {
    dentry->list[i] = (struct dentry*)fixed_obj_allocate(d_token);
    dentry->list[i]->type = NONE;
    dentry->list[i]->name[0] = 'N';
    dentry->list[i]->name[1] = '\0';
  }

  root->v_ops = tmpfs_v_ops;
  root->f_ops = tmpfs_f_ops;
  root->internal = (void*)dentry;
  mount->root = root;
  return 1;
}

int tmpfs_lookup(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
  for (int i = 0; i < DIR_MAX; i++) {
    if (!strcmp(((struct dentry*)dir_node->internal)->list[i]->name,
                component_name)) {
      struct dentry* dentry = ((struct dentry*)dir_node->internal)->list[i];
      *target = dentry->vnode;
      return 1;
    }
  }
  return -1;
}

int tmpfs_create(struct vnode* dir_node, struct vnode** target,
                 const char* component_name) {
  for (int i = 0; i < DIR_MAX; i++) {
    if (((struct dentry*)dir_node->internal)->list[i]->type == NONE) {
      struct dentry* dentry = ((struct dentry*)dir_node->internal)->list[i];
      strcpy(dentry->name, component_name);
      dentry->type = FILE;
      struct vnode* vnode = (struct vnode*)fixed_obj_allocate(v_token);
      vnode->v_ops = dir_node->v_ops;
      vnode->f_ops = dir_node->f_ops;
      vnode->internal = dentry;
      init_dentry(dentry, vnode, component_name);
      dentry->buf = fixed_obj_allocate(tb_token);
      dentry->buf->size = 0;

      *target = dentry->vnode;
      return 1;
    }
  }
  return -1;
}

int tmpfs_write(struct file* file, const void* buf, unsigned long len) {
  struct dentry* internal = (struct dentry*)file->vnode->internal;
  for (int i = 0; i < len; i++) {
    internal->buf->buffer[file->f_pos++] = ((char*)buf)[i];
    if (internal->buf->size < file->f_pos)
      internal->buf->size = file->f_pos;
  }

}

int tmpfs_read(struct file* file, void* buf, unsigned long len) {
  unsigned long size = 0;
  struct dentry* internal = (struct dentry*)file->vnode->internal;
  for (size= 0; size < len; size++) {
    ((char*)buf)[size] = internal->buf->buffer[file->f_pos++];
    if (size == internal->buf->size)
      break;
  }
  return size;
}

void tmpfs_ls(struct vnode* dir_node, const char* component_name, const char* parent_name) {
  char* dentryname = ((struct dentry*)dir_node->internal)->name;
  char* parentname = parent_name;
  strcat(parentname, dentryname);
  for (int i = 0; i < DIR_MAX; i++) {
    struct dentry* internal = ((struct dentry*)dir_node->internal)->list[i];
    if (internal->type == DICTIONARY)
      tmpfs_ls(internal->vnode, component_name, parentname);
    else {
      char* filename = internal->name;
      uart_puts(component_name);
      uart_puts(parentname);
      uart_puts(filename);
      uart_puts("\n");
    }
  }
}

void tmpfs_mkdir(struct vnode* dir_node, const char* component_name) {
  struct dentry* dentry = (struct dentry*)fixed_obj_allocate(d_token);
  struct vnode* vnode = (struct vnode*)fixed_obj_allocate(v_token);
  struct vnode* root= (struct vnode*)fixed_obj_allocate(v_token);
  vnode->v_ops = tmpfs_v_ops;
  vnode->f_ops = tmpfs_f_ops;

  dentry->type = DICTIONARY;
  dentry->vnode = vnode;

  init_dentry(dentry, vnode, "/");
  for (int i = 0; i < DIR_MAX; i++) {
    dentry->list[i] = (struct dentry*)fixed_obj_allocate(d_token);
    dentry->list[i]->type = NONE;
    dentry->list[i]->name[0] = 'N';
  }

  root->v_ops = tmpfs_v_ops;
  root->f_ops = tmpfs_f_ops;
  root->internal = (void*)dentry;

}
