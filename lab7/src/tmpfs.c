
#include "io.h"
#include "tmpfs.h"
#include "string.h"
#include "allocator.h"


typedef struct TmpfsfdTag{
  const char *name;
  enum dirent_type type;
  struct vnode *parent;
  union{
    char *text; 
    struct vnode *child;
  };
  struct vnode *next;
} *Tmpfsfd, TmpfsfdStr;

Tmpfsfd newTmpfsFile(const char *name, struct vnode *parent){
  Tmpfsfd new = (Tmpfsfd)kmalloc(sizeof(TmpfsfdStr));
  new->name = name;
  new->parent = parent;
  new->type = dirent_file;
  new->text = (char*)kmalloc(512);
  new->next = NULL;
  return new;
}

Tmpfsfd newTmpfsDir(
    const char *name,
    struct vnode *parent,
    struct vnode *child,
    struct vnode *next
    ){
  Tmpfsfd new = (Tmpfsfd)kmalloc(sizeof(TmpfsfdStr));
  new->name = name;
  new->parent = parent;
  new->type = dirent_dir;
  new->child = child;
  new->next = next;
  return new;
}

#define Tmpfsfd(x) ((Tmpfsfd)(x)->internal)

#define TmpfsfdChild(x) \
  (((Tmpfsfd)(x)->internal)->type == dirent_dir ? \
   ((Tmpfsfd)(x)->internal)->child : NULL)

#define TmpfsIsFile(n) \
  (Tmpfsfd(n) && Tmpfsfd(n)->type == dirent_file)

#define TmpfsIsDir(n) \
  (Tmpfsfd(n) && Tmpfsfd(n)->type == dirent_dir)

typedef struct TmpfsFileTag{
  char *name;
  struct TmpfsFileTag *next;
} *TmpfsFile, TmpfsFileStr;


int tmpfs_setup_mount(struct filesystem *fs, struct mount *mount);

int tmpfs_write(struct file *file, const void *buf, size_t len);
int tmpfs_read(struct file *file, void *buf, size_t len);

DIR *tmpfs_opendir(struct vnode *node,
    DIR *dir, const char *pathname);

dirent *tmpfs_readdir(DIR *directory);

int tmpfs_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int tmpfs_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
enum dirent_type tmpfs_typeof(struct vnode *node);

struct filesystem *tmpfs = &(struct filesystem){
  .name = "tmpfs",
    .setup_mount = tmpfs_setup_mount,
    .nextfs = NULL, 
};

struct file_operations *tmpfs_fop =
&(struct file_operations){
  .write = tmpfs_write,
  .read = tmpfs_read,
};

struct directory_operations *tmpfs_dop =
&(struct directory_operations){
  .opendir = tmpfs_opendir,
  .readdir = tmpfs_readdir,
};


struct vnode_operations *tmpfs_vop =
&(struct vnode_operations){
  .lookup = tmpfs_lookup,
  .create = tmpfs_create,
  .typeof = tmpfs_typeof,
};

int tmpfs_setup_mount(
    struct filesystem *fs, struct mount *mount){
  mount->fs = fs;
  mount->root = newVnode(
      NULL,
      tmpfs_vop,
      tmpfs_fop,
      tmpfs_dop,
      newTmpfsDir("", NULL, NULL, NULL));
  return 0;
}

void print_ident(int indent){
  while(indent--) printf(" ");
}

void tmpfs_show_tree(
    struct vnode *dir_node, int indent){

  print_ident(indent);
  printf("{%s}" NEWLINE,
      *Tmpfsfd(dir_node)->name ?
      Tmpfsfd(dir_node)->name : "/");

  struct vnode *child = TmpfsfdChild(dir_node);
  while(child){
    if(TmpfsIsDir(child)){
      tmpfs_show_tree(child, indent + 2);
    }
    else{
      print_ident(indent + 2);
      printf("<%s>" NEWLINE, Tmpfsfd(child)->name);
    }
    child = Tmpfsfd(child)->next;
  }
}

int subpath_of(const char *sub, const char *full){
  while(*sub){
    if(*full == *sub) full++, sub++;
    else break;
  }
  return (*full == '/' || *full == 0) && *sub == 0;
}

int tmpfs_lookup(
    struct vnode *node,
    struct vnode **target,
    const char *component_name){
  if(*component_name == '/') component_name++;
  if(!*component_name){
    *target = node;
    return 1;
  }

  struct vnode *child = TmpfsfdChild(node);

  while(child){
    if(subpath_of(Tmpfsfd(child)->name, component_name)){
      return tmpfs_lookup(child, target, component_name + strlen(Tmpfsfd(child)->name));
    }
    child = Tmpfsfd(child)->next;
  }
  *target = 0;
  return 0;
}

int exist_slash(const char *path){
  while(*path)
    if(*path == '/') return 1;
    else path++;
  return 0;
}

int tmpfs_create(
    struct vnode *dir_node,
    struct vnode **target,
    const char *component_name){

  if(!TmpfsIsDir(dir_node)) return 0;

  if(*component_name == '/') component_name++;
  if(!*component_name){
    *target = 0;
    return 0; /* create existed file */
  }
  struct vnode **childptr = &(Tmpfsfd(dir_node)->child);
  while(*childptr){
    if(subpath_of(Tmpfsfd(*childptr)->name, component_name)){
      return tmpfs_create(*childptr, target, component_name + strlen(Tmpfsfd(*childptr)->name));
    }
    childptr = &(Tmpfsfd(*childptr)->next);
  }
  if(exist_slash(component_name)) return 0;

  *childptr =
    newVnode(NULL,
        dir_node->v_ops,
        dir_node->f_ops,
        dir_node->d_ops,
        newTmpfsFile(component_name, dir_node));

  *target = *childptr;
  return 1;
}

int tmpfs_write(struct file *file, const void *buf, size_t len){
  char *ctx = Tmpfsfd(file->vnode)->text;
  char *pos = ctx + file->f_pos, *ptr = (char*)buf;

  while(len--){
    *pos = *ptr;
    pos++, ptr++;
  }
  *pos = EOF;
  return 0;
}

int tmpfs_read(struct file *file, void *buf, size_t len){
  char *ctx = Tmpfsfd(file->vnode)->text;
  char *pos = ctx + file->f_pos, *ptr = (char*)buf;

  int rlen = 0;
  while(len-- && *pos != EOF){
    rlen += 1;
    *ptr = *pos;
    pos++, ptr++;
  }
  return rlen;
}

DIR *tmpfs_opendir(
    struct vnode *node,
    DIR *dir, const char *pathname){
  dir->path = pathname; // should use strdup
  dir->entry.name = NULL;
  dir->entry.type = dirent_none;
  dir->child = NULL;
  dir->dops = tmpfs_dop;
  struct vnode *target;
  if(tmpfs_lookup(node, &target, pathname)){
    dir->root = target;
    dir->child = TmpfsfdChild(target);
    return dir;
  }
  return NULL;
}

dirent *tmpfs_readdir(DIR *dir){
  if(!dir->child) return NULL;
  Tmpfsfd p = Tmpfsfd(dir->child);
  dir->entry.name = p->name;
  dir->entry.type = p->type;
  dir->child = p->next;
  return &(dir->entry);
}

enum dirent_type tmpfs_typeof(struct vnode *node){
  return Tmpfsfd(node)->type;
}
