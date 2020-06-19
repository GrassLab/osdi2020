#include "io.h"
#include "type.h"
#include "util.h"
#include "fat32.h"
#include "sdhost.h"
#include "string.h"
#include "allocator.h"

typedef struct Fat32fdTag{
  const char *name;
  enum dirent_type type;
  struct vnode *parent;
  struct SFN_entry entry;
  union{
    char *text;
    struct vnode *child;
  };
  struct vnode *next;
} *Fat32fd, Fat32fdStr;

void parse_filename(const char *name, char *fn, char *ext){
  char *p = fn;
  for(int i = 0; i < 8; i++, p++, name++)
    if(*name != ' ') *p = *name;
    else break;
  while(*name == ' ') name++;
  if(fn == ext) *p++ = '.';
  else *p = 0, p = ext;
  for(int i = 0; i < 3; i++, p++, name++)
    if(*name != ' ' && *name) *p = *name;
    else break;
  *p = 0;
}

void encode_filename(const char *name, char *code){
  char *c = code;
  for(int i = 0; i < 8; i++){
    if(*name == '.') break;
    if(*name == ' ') name++;
    *c = *name;
    c++, name++;
  }
  while(*name && *name != '.') name++;
  while(c < code + 8) *c++ = ' ';
  if(*name == '.'){
    name++;
    for(int i = 0; i < 3; i++){
      if(!*name) break;
      *c = *name;
      c++, name++;
    }
  }
}

Fat32fd newFat32File(struct SFN_entry *entry, struct vnode *parent){
  Fat32fd new = (Fat32fd)kmalloc(sizeof(Fat32fdStr));
  new->entry = *entry;
  char mstr[12];
  parse_filename(new->entry.filename, mstr, mstr);
  new->name = strdup(mstr); // todo, release the mem
  new->parent = parent;
  new->type = dirent_file;
  new->text = NULL;
  new->next = NULL;
  return new;
}

Fat32fd newFat32Dir(
    struct SFN_entry *entry,
    struct vnode *parent,
    struct vnode *child,
    struct vnode *next
    ){
  Fat32fd new = (Fat32fd)kmalloc(sizeof(Fat32fdStr));
  new->name = strdup(entry->filename); // todo, release the mem
  new->parent = parent;
  new->type = dirent_dir;
  new->child = child;
  new->next = next;
  return new;
}

#define Fat32fd(x) ((Fat32fd)(x)->internal)

#define Fat32fdChild(x) \
  (((Fat32fd)(x)->internal)->type == dirent_dir ? \
   ((Fat32fd)(x)->internal)->child : NULL)

#define Fat32IsFile(n) \
  (Fat32fd(n) && Fat32fd(n)->type == dirent_file)

#define Fat32IsDir(n) \
  (Fat32fd(n) && Fat32fd(n)->type == dirent_dir)

typedef struct Fat32FileTag{
  char *name;
  struct Fat32FileTag *next;
} *Fat32File, Fat32FileStr;

int fat32_setup_mount(struct filesystem *fs, struct mount *mount);

// file op
int fat32_write(struct file *file, const void *buf, size_t len);
int fat32_read(struct file *file, void *buf, size_t len);

// dir op
DIR *fat32_opendir(struct vnode *node,
    DIR *dir, const char *pathname);
dirent *fat32_readdir(DIR *directory);
int fat32_mkdir(struct vnode *node, const char *pathname);
int fat32_chdir(struct vnode *node, const char *pathname);

// vnode op
int fat32_lookup(struct vnode *dir_node, struct vnode **target, const char *component_name);
int fat32_create(struct vnode *dir_node, struct vnode **target, const char *component_name);
enum dirent_type fat32_typeof(struct vnode *node);

struct filesystem *fat32 =
&(struct filesystem){
  .name = "fat32",
  .setup_mount = fat32_setup_mount,
  .nextfs = NULL,
};

struct file_operations *fat32_fop =
&(struct file_operations){
  .write = fat32_write,
  .read = fat32_read,
};

struct directory_operations *fat32_dop =
&(struct directory_operations){
  .opendir = fat32_opendir,
  .readdir = fat32_readdir,
  .mkdir   = fat32_mkdir,
  .chdir   = fat32_chdir,
};

struct vnode_operations *fat32_vop =
&(struct vnode_operations){
  .lookup = fat32_lookup,
  .create = fat32_create,
  .typeof = fat32_typeof,
};

/*
 * www.cs.fsu.edu/~cop4610t/lectures/project3/Week11/Slides_week11.pdf
 * http://read.pudn.com/downloads228/sourcecode/embed/1070968/fat.c__.htm
 * https://doxygen.reactos.org/d7/d53/bootblock_8h_source.html
 */
void list_block(char *buffer){
  for(int i = 0; i < 512; i++)
    printf("%s%2x%s", i % 16 == 0 ? NEWLINE : "",
        buffer[i], i % 2 ? " " : "");
  puts("");
}

void show_sec(size_t index){
  char buffer[512]; readblock(index, buffer); list_block(buffer); 
}

typedef struct fat32_info{
  unsigned long FirstDataSector;
  unsigned long SectorsPerCluster;
  unsigned long BytesPerSector;  
  unsigned long FirstFATSector;     
} *FAT32_INFO, FAT32_INFO_STR;

FAT32_INFO info;
struct mbr_partition *pentry;
struct mbr_bpbFAT32 *bpb;

void fat32_init(){

  info = (FAT32_INFO)kmalloc(sizeof(FAT32_INFO_STR));

  char buffer[512];
  readblock(0, buffer);

  /* use struct here becuase buffer will be replaced */
  pentry = (struct mbr_partition *)kmalloc(sizeof(struct mbr_partition));
  *pentry = ((struct mbr_sector*)buffer)->mbr_parts[0];

  readblock(pentry->mbrp_start, buffer);

  /* note bpb cannot be used if buffer replaced latter */
  bpb = (struct mbr_bpbFAT32 *)kmalloc(sizeof(struct mbr_bpbFAT32));

  *bpb = ((struct mbr_sector*)buffer)->mbr_bpb;

  info->FirstDataSector = pentry->mbrp_start;
  if(bpb->bpbFATsecs){   
    // bpbFATsecs is non-zero and is therefore valid   
    info->FirstDataSector +=
      bpb->bpbResSectors +
      bpb->bpbFATs * bpb->bpbFATsecs;
  }   
  else{   
    // bpbFATsecs is zero, real value is in bpbBigFATsecs   
    info->FirstDataSector +=
      bpb->bpbResSectors + 
      bpb->bpbFATs * bpb->bpbBigFATsecs;
  }   
  info->SectorsPerCluster   = bpb->bpbSecPerClust;   
  info->BytesPerSector      = bpb->bpbBytesPerSec;   
  info->FirstFATSector      = bpb->bpbResSectors + pentry->mbrp_start;
}

char *read_file_ctx(int N, char *buffer){
  unsigned long sector = pentry->mbrp_start;
  sector += bpb->bpbResSectors +  N * bpb->bpbBigFATsecs;
  readblock(info->FirstDataSector, buffer);
  return buffer;
}

char *write_file_ctx(int N, char *buffer){
  unsigned long sector = pentry->mbrp_start;
  sector += bpb->bpbResSectors +  N * bpb->bpbBigFATsecs;
  writeblock(info->FirstDataSector, buffer);
  return buffer;
}

void append_directory_entry(struct vnode *dir, struct SFN_entry *entry){
  char buffer[512];
  struct SFN_entry *e = &(Fat32fd(dir)->entry);
  read_file_ctx(e->start_lo | (e->start_hi << 16), buffer);
  struct SFN_entry *fentry = (struct SFN_entry*)buffer;
  while(*(fentry->filename)) fentry += 1;
  *fentry = *entry;
  write_file_ctx(e->start_lo | (e->start_hi << 16), buffer);
}

struct vnode *fat32_build_file(struct SFN_entry *entry, struct vnode *parent){
  return newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
      newFat32File(entry, parent));
}

struct vnode *fat32_build_dir(struct SFN_entry *entry, struct vnode *parent){

  struct vnode *dir = newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
      newFat32Dir(entry, parent, NULL, NULL));

  char *buffer = (char*)kmalloc(sizeof(char) * 512);

  read_file_ctx(entry->start_lo | (entry->start_hi << 16), buffer);

  struct SFN_entry *fentry = (struct SFN_entry*)buffer;

  struct vnode **iter = &(Fat32fd(dir)->child); 

  while(*fentry->filename){
    printfmt("%s %2x %2x",
        fentry->filename,
        fentry->start_hi,
        fentry->start_lo);
    if(fentry->attr & 0x10){
      *iter = newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
          newFat32File(fentry, dir));
    }
    else{
      *iter = fat32_build_file(fentry, dir);
    }
    iter = &(Fat32fd(*iter)->next);
    fentry += 1;
  }
  *iter = NULL;
  kfree(buffer);
  return dir;
}

/* http://www.c-jump.com/CIS24/Slides/FAT/F01_0180_sfn.htm */
struct vnode *fat32_build_root(){
  fat32_init();
  struct SFN_entry root_entry = {
    .filename = "/", .attr = 0x10, 
    .reserved = 0, .create_ms = 0, .create_hms = 0,
    .date = 0, .access_date = 0,
    .start_hi = 0, .modify_time = 0, .modify_date = 0,
    .start_lo = info->FirstDataSector, .size = 0
  };
  return fat32_build_dir(&root_entry, NULL);
}

/* vfs impl below */

void fat32_show_tree(
    struct vnode *dir_node, int indent){

  print_ident(indent);
  printf("{%s}" NEWLINE,
      Fat32fd(dir_node)->name);

  struct vnode *child = Fat32fdChild(dir_node);
  while(child){
    if(Fat32IsDir(child)){
      fat32_show_tree(child, indent + 2);
    }
    else{
      print_ident(indent + 2);
      printf("<%s>" NEWLINE, Fat32fd(child)->name);
    }
    child = Fat32fd(child)->next;
  }
}

int fat32_lookup(
    struct vnode *node,
    struct vnode **target,
    const char *component_name){
  if(*component_name == '/') component_name++;

  if(!*component_name){
    *target = node;
    return 1;
  }

  if(subpath_of(".", component_name)){
    return node->v_ops->lookup(
        node, target,
        component_name + strlen("."));
  }
  else if(subpath_of("..", component_name)){
    if(node->mount && node->mount->mp){
      if(node->mount->root != node){
        struct vnode *parent = Fat32fd(node)->parent;
        return parent->v_ops->lookup(
            parent, target,
            component_name + strlen("..")
            );
      } 
      else{
        return node->mount->mp->v_ops->lookup(
            node->mount->mp, target,
            component_name
            );
      }

    }
    else{
      struct vnode *parent = Fat32fd(node)->parent;
      if(parent)
        return parent->v_ops->lookup(
            parent, target,
            component_name);
      else return 0;
    }
  }
  else if(node->mount && node->mount->root != node){
    return node->mount->root->v_ops->lookup(
        node->mount->root, target, component_name);
  }

  if(!*component_name){
    *target = node;
    return 1;
  }

  struct vnode *child = Fat32fdChild(node);
  while(child){
    if(subpath_of(Fat32fd(child)->name, component_name)){
      return child->v_ops->lookup(
          child, target,
          component_name + strlen(Fat32fd(child)->name));
    }
    child = Fat32fd(child)->next;
  }
  *target = 0;
  return 0;
}

int fat32_create(
    struct vnode *dir_node,
    struct vnode **target,
    const char *component_name){

  if(!Fat32IsDir(dir_node)) return 0;

  if(*component_name == '/') component_name++;
  if(!*component_name){
    *target = 0;
    return 0; /* create existed file */
  }
  struct vnode **childptr = &(Fat32fd(dir_node)->child);
  while(*childptr){
    if(subpath_of(Fat32fd(*childptr)->name, component_name)){
      return fat32_create(*childptr, target, component_name + strlen(Fat32fd(*childptr)->name));
    }
    childptr = &(Fat32fd(*childptr)->next);
  }
  if(exist_slash(component_name)) return 0;

  char code[11];

  encode_filename(component_name, code);

  struct SFN_entry new_entry = {
    .attr = 0x10, 
    .reserved = 0, .create_ms = 0, .create_hms = 0,
    .date = 0, .access_date = 0,
    .start_hi = 0, .modify_time = 0, .modify_date = 0,
    .start_lo = info->FirstDataSector, .size = 0
  };

  for(int i = 0; i < 11; i++)
    new_entry.filename[i] = code[i];

  *childptr =
    newVnode(NULL,
        dir_node->v_ops,
        dir_node->f_ops,
        dir_node->d_ops,
        newFat32File(&new_entry, dir_node));

  append_directory_entry(dir_node, &new_entry);

  *target = *childptr;
  return 1;
}

int fat32_write(struct file *file, const void *buf, size_t len){
  char ctx[512];
  struct SFN_entry *entry = &(Fat32fd(file->vnode)->entry);
  read_file_ctx(entry->start_lo | (entry->start_hi << 16), ctx);
  char *pos = ctx + file->f_pos, *ptr = (char*)buf;
  while(len--){
    *pos = *ptr;
    pos++, ptr++;
  }
  *pos = EOF;
  write_file_ctx(entry->start_lo | (entry->start_hi << 16), ctx);
  return 0;
}

int fat32_read(struct file *file, void *buf, size_t len){
  char ctx[512];
  struct SFN_entry *entry = &(Fat32fd(file->vnode)->entry);
  read_file_ctx(entry->start_lo | (entry->start_hi << 16), ctx);
  char *pos = ctx + file->f_pos, *ptr = (char*)buf;
  int rlen = 0;
  while(len-- && *pos != EOF){
    rlen += 1;
    *ptr = *pos;
    pos++, ptr++;
  }
  return rlen;
}

DIR *fat32_opendir(
    struct vnode *node,
    DIR *dir, const char *pathname){
  dir->path = strdup(pathname); // should use strdup
  dir->entry.type = dirent_none;
  dir->child = NULL;
  dir->dops = fat32_dop;
  struct vnode *target;
  if(node->v_ops->lookup(node, &target, pathname)){
    target = move_mount_root(target);
    //if(target->v_ops->typeof(target) == dirent_dir){
    dir->root = target;
    dir->entry.name = strdup(Fat32fd(target)->name);
    dir->child = Fat32fdChild(target);
    return dir;
    //}
  }
  return NULL;
}

dirent *fat32_readdir(DIR *dir){
  if(!dir->child) return NULL;
  Fat32fd p = Fat32fd(dir->child);
  dir->entry.name = p->name;
  dir->entry.type = p->type;
  dir->child = p->next;
  return &(dir->entry);
}

enum dirent_type fat32_typeof(struct vnode *node){
  return Fat32fd(node)->type;
}

int fat32_mkdir(struct vnode *node, const char *pathname){
  int len = strlen(pathname), name = 0;
  char path[len], *p = path + len - 1, *newp = NULL;
  strcpy(path, pathname);
  while(p > path){
    if(*p == ' ') p--;
    else if(*p == '/'){
      if(name) break;
      else p--;
    }
    else name = 1, p--;
  }
  if(p != path) *p = 0, newp = p + 1, p = path;
  else newp = path, p = path + len;
  while(*newp == '/') newp++;

  struct vnode *target;
  if(node->v_ops->lookup(node, &target, pathname)){
    // existed directory
    return 0;
  }
  if(node->v_ops->lookup(node, &target, p)){
    target = move_mount_root(target);
    Fat32fd(target)->child =  newVnode(
        NULL, fat32_vop, fat32_fop, fat32_dop,
        newFat32Dir( // todo, replace fentry here
          NULL, target, NULL, Fat32fd(target)->child));
  }
  return 0;
}

int fat32_chdir(struct vnode *node, const char *pathname){
  struct vnode *target;
  if(node->v_ops->lookup(node, &target, pathname)){
    target = move_mount_root(target);
    printfmt("pwd <%x>", target);
    current_task->pwd = target;
    return 0;
  } else puts("no such file or directory");
  return 1;
}

int fat32_setup_mount(
    struct filesystem *fs, struct mount *mount){
  mount->fs = fs;
  if(!mount->root){
    mount->root = fat32_build_root(); 
    Fat32fd(mount->root)->parent = mount->root;
    mount->root->mount = mount;
    printfmt("fat32 mount on 0x%x", mount->mp);
  }
  if(!fs->mnt) fs->mnt = mount;
  return 0;
}
