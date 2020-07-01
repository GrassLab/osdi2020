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

int upper_cmp(char a, char b){
  return toupper(a) == toupper(b);
}

void parse_filename(const char *name, char *fn, char *ext){
  char *p = fn;
  for(int i = 0; i < 8; i++, p += p ? 1 : 0, name++)
    if(*name != ' ' && p) *p = *name;
    else break;
  while(*name == ' ') name++;
  if(p){
    if(fn == ext) *p++ = '.';
    else *p = 0, p = ext;
  } else p = ext;
  for(int i = 0; i < 3; i++, p += p ? 1 : 0, name++)
    if(*name != ' ' && *name && p) *p = *name;
    else break;
  if(p) *p = 0;
}

void encode_filename(const char *name, char *code){
  char *c = code;
  for(int i = 0; i < 8; i++){
    if(*name == '.' || !(*name)) break;
    if(*name == ' ') name++;
    *c = toupper(*name);
    c++, name++;
  }
  while(*name && *name != '.') name++;
  while(c < code + 8) *c++ = ' ';
  if(*name == '.'){
    name++;
    for(int i = 0; i < 3; i++){
      if(!*name) break;
      *c = toupper(*name);
      c++, name++;
    }
  }
  else{
    for(int i = 0; i < 3; i++) *c = ' ', c++;
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

Fat32fd newFat32LongFile(char *name, struct SFN_entry *entry, struct vnode *parent){
  Fat32fd new = (Fat32fd)kmalloc(sizeof(Fat32fdStr));
  new->entry = *entry;
  char mstr[12];
  parse_filename(new->entry.filename, mstr, mstr);
  new->name = name; // todo, release the mem
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
  char mstr[16];
  parse_filename(entry->filename, mstr, NULL);
  new->name = strdup(mstr); // todo, release the mem
  new->parent = parent;
  new->entry = *entry;
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
        buffer[i], i % 4 == 3 ? "    " : (i % 2 ? "  " : " "));
  puts("");
}

void show_sec(size_t index){
  char buffer[512]; readblock(index, buffer); list_block(buffer);
}

int checkAllZero(size_t index){
  char buffer[512];
  readblock(index, buffer);
  //list_block(buffer);
  for(int i = 0; i < 512; i++){
    if(buffer[i]) return 0;
  }
  return 1;
}

typedef struct fat32_info{
  char *fat;
  unsigned long FirstDataSector;
  unsigned long FirstFATSector;
} *FAT32_INFO, FAT32_INFO_STR;

FAT32_INFO info = NULL;
struct unpacked_mbr_bpbFAT32 *bpb = NULL;

unsigned long fatClustToSect(unsigned long clust){
  return ((clust-2) * bpb->bpbSecPerClust) + info->FirstDataSector;
}

#define CLUST_EOFE      0x0fffffff      // end of eof cluster range
#define FAT32_MASK      0x0fffffff      // mask for FAT32 cluster numbers
unsigned fat32_next_cluster(unsigned long cluster, unsigned next){
  unsigned long fatOffset = cluster << 2;
  unsigned long fatMask = FAT32_MASK;
  unsigned long sector = info->FirstFATSector + (fatOffset / bpb->bpbBytesPerSec);
  unsigned long offset = fatOffset % bpb->bpbBytesPerSec;
  readblock(sector, info->fat);
  if(next){
    *((unsigned*) &(info->fat[offset])) = next;
    writeblock(sector, info->fat);
    return *((unsigned*) &(info->fat[offset]));
  }
  else{
    unsigned nextCluster = (*((unsigned*) &(info->fat[offset]))) & fatMask;
    return (nextCluster & 0xFFFFFF8) == 0xFFFFFF8 ? 0 : nextCluster;
  }
}

unsigned long fat32_show_next_cluster(unsigned long cluster){
  unsigned long fatOffset = cluster << 2;
  //unsigned long fatMask = FAT32_MASK;
  unsigned long sector = info->FirstFATSector + (fatOffset / bpb->bpbBytesPerSec);
  show_sec(sector);
  return 0;
}

#define FAT32_EOC 0x0FFFFFFF
#define FAT32_AVAIL_MASK 0x0FFFFFFF

#define end_FAT_sector (info->FirstFATSector + bpb->bpbFATs)
unsigned fat32_avail_cluster(unsigned set_val){
  static int sector = 0;
  if(!sector) sector = info->FirstFATSector;

  readblock(sector, info->fat);
  while(sector < info->FirstDataSector){
    //printfmt("check sector %d end %d", sector, info->FirstDataSector);
    readblock(sector, info->fat);
    //list_block(info->fat);
    for(int i = 0, *cluster = (int*)info->fat;
        i < bpb->bpbBytesPerSec / 4; i++, cluster++){
      unsigned theN = ((sector - info->FirstFATSector) * (bpb->bpbBytesPerSec / 4) + i);
      //printfmt("(%d) %d.%d %x %x",
      //    theN,
      //    sector, i, *cluster, *cluster & FAT32_AVAIL_MASK);
      if((*cluster & FAT32_AVAIL_MASK) == 0){
        if(!checkAllZero(fatClustToSect(theN))) continue;

        if(set_val){
          *cluster = set_val;
          writeblock(sector, info->fat);
        }
        return theN;
      }
    }
    sector++;
  }
  return 0;
}

void fat32_init(){

  info = (FAT32_INFO)kmalloc(sizeof(FAT32_INFO_STR));

  char *buffer = (char*)kmalloc(512 * sizeof(char));

  info->fat = buffer; // give fat the space for latter use

  readblock(0, buffer);

  /* use struct here becuase buffer will be replaced */

  int mbrp_start_offset =
    sizeof(struct mbr_sector)
    - sizeof(struct mbr_partition) * MBR_PART_COUNT
    - sizeof(short) + sizeof(char) * 8;

  int mbrp_start = read_le(buffer + mbrp_start_offset, 4);

  printfmt("start is %d", mbrp_start);

  readblock(mbrp_start, buffer);

  /* note bpb cannot be used if buffer replaced latter */
  bpb = (struct unpacked_mbr_bpbFAT32 *)kmalloc(sizeof(struct unpacked_mbr_bpbFAT32));

  //memcpy((unsigned long)buffer + 11, (unsigned long)bpb, sizeof(struct mbr_bpbFAT32));
  //*bpb = ((struct mbr_sector*)buffer)->mbr_bpb;

#define read_bpb_entry(buf, bpbptr, entry, offset) \
  bpbptr->entry = read_le( \
      buf + offset, \
      sizeof(bpbptr->entry))

  read_bpb_entry(buffer, bpb, bpbBytesPerSec, 11);
  read_bpb_entry(buffer, bpb, bpbSecPerClust, 13);
  read_bpb_entry(buffer, bpb, bpbResSectors , 14);
  read_bpb_entry(buffer, bpb, bpbFATs       , 16);
  read_bpb_entry(buffer, bpb, bpbFATsecs    , 22);
  read_bpb_entry(buffer, bpb, bpbBigFATsecs , 36);
  read_bpb_entry(buffer, bpb, bpbRootClust  , 44);

#if 0
#define check(entry) \
  if(bpb->entry != ((struct mbr_sector*)buffer)->mbr_bpb.entry) \
  puts(str(entry) "not the same");

  check(bpbBytesPerSec);
  check(bpbSecPerClust);
  check(bpbResSectors );
  check(bpbFATs       );
  check(bpbFATsecs    );
  check(bpbBigFATsecs );
  check(bpbRootClust  );
#endif

  info->FirstDataSector = mbrp_start;
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
  info->FirstFATSector      = bpb->bpbResSectors + mbrp_start;
  //unsigned n = fat32_avail_cluster(0);
  //printfmt("avail is %d", n);
  // 5860 0x16e4
  //show_sec(0);
}

#define entry_cluster_num(e) \
  ((e)->start_lo | ((e)->start_hi << 16))

#define read_entry_ctx(e, ctx) \
  read_file_ctx(entry_cluster_num(e), ctx);

char *read_file_ctx(int N, char *buffer){
  readblock(fatClustToSect(N), buffer);
  return buffer;
}

#define write_entry_ctx(e, ctx) \
  write_file_ctx((e)->start_lo | ((e)->start_hi << 16), ctx);

char *write_file_ctx(int N, char *buffer){
  writeblock(fatClustToSect(N), buffer);
  return buffer;
}

void show_entry_name(char *s){
  for(int i = 0; i < 11; i++)
    printf("%c", s[i]);
}

void read_sfn_entry(char *p, struct SFN_entry *entry){

  for(int i = 0; i < 11; i++, p++)
    entry->filename[i] = *p;
  entry->attr = *p, p++;
  entry->reserved = *p, p++;
  entry->create_ms = *p, p++;
#define read_entry(attr) \
  entry->attr  = read_le(p, sizeof(entry->attr)), \
  p += sizeof(entry->attr)
  read_entry(create_hms);
  read_entry(date);
  read_entry(access_date);
  read_entry(start_hi);
  read_entry(modify_time);
  read_entry(modify_date);
  read_entry(start_lo);
  read_entry(size);
#undef read_entry
}

void write_sfn_entry(char *p, struct SFN_entry *entry){
  for(int i = 0; i < 11; i++, p++)
    *p = entry->filename[i];
  *p = entry->attr, p++;
  *p = entry->reserved, p++;
  *p = entry->create_ms, p++;
#define write_entry(attr) \
  write_le(p, entry->attr , sizeof(entry->attr)); \
  p += sizeof(entry->attr)
  write_entry(create_hms );
  write_entry(date       );
  write_entry(access_date);
  write_entry(start_hi   );
  write_entry(modify_time);
  write_entry(modify_date);
  write_entry(start_lo   );
  write_entry(size       );
}

int append_directory_entry(struct vnode *dir, struct SFN_entry *entry){
  char *buffer = (char*)kmalloc(sizeof(char) * 512);
  struct SFN_entry *e = &(Fat32fd(dir)->entry);
  int N = entry_cluster_num(e), prevN = 0;
  while(N){
    read_file_ctx(N, buffer);
    struct SFN_entry fentry;
    for(int i = 0; i < 16; i++){
      read_sfn_entry(buffer + i * 32, &fentry);
      if(*fentry.filename == 0xe5 || *fentry.filename == 0x00){
        write_sfn_entry(buffer + i * 32, entry);
        //printfmt("pass %x %s", *fentry.filename, fentry.filename);
        write_file_ctx(N, buffer);
        kfree(buffer);
        return 0;
      } //else printfmt("pass %s", fentry.filename);
    }
    prevN = N;
    N = fat32_next_cluster(N, 0);
  }
  puts("directory don't have enough space");
  // directory need next cluster
  unsigned nextN = fat32_avail_cluster(FAT32_EOC);
  fat32_next_cluster(prevN, nextN);
  read_file_ctx(nextN, buffer);
  struct SFN_entry fentry;
  for(int i = 0; i < 16; i++){
    read_sfn_entry(buffer + i * 32, &fentry);
    if(*fentry.filename == 0xe5 || *fentry.filename == 0x00){
      write_sfn_entry(buffer + i * 32, entry);
      write_file_ctx(nextN, buffer);
      kfree(buffer);
      return 0;
    }
  }
  printfmt("append directory failed");
  kfree(buffer);
  return 1;
}

int update_file_metadata(struct vnode *node, struct SFN_entry *update_entry){

  char *buffer = (char*)kmalloc(512 * sizeof(char));
  read_entry_ctx(&(Fat32fd(node)->entry), buffer);
  struct SFN_entry *entry = &(Fat32fd(node)->entry);
  int N = entry_cluster_num(entry);
  while(N){
    read_file_ctx(N, buffer);
    struct SFN_entry fentry;
    for(int i = 0; i < 16; i++){
      read_sfn_entry(buffer + i * 32, &fentry);
      if(!strncmp(fentry.filename, update_entry->filename, 11)){
        write_sfn_entry(buffer + i * 32, update_entry);
        write_file_ctx(N, buffer);
        kfree(buffer);
        return 0;
      }
    }
    N = fat32_next_cluster(N, 0);
  }
  puts("update entry not found");
  kfree(buffer);
  return 1;
}

struct vnode *fat32_build_file(struct SFN_entry *entry, struct vnode *parent){
  //printfmt("file N - %d", entry_cluster_num(entry));
  return newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
      newFat32File(entry, parent));
}

char *parse_long_filename(char *ptr, char *beg){
  char name[128];
  char *p = name;
  while(ptr != beg){
    for(int i = 1; i < 7; i++)
      *p = *(ptr + i), p++;
    *p = *(ptr + 9), p++;
    *p = *(ptr + 10), p++;
    for(int i = 14; i < 22; i++)
      *p = *(ptr + i), p++;
    *p = *(ptr + 24), p++;
    *p = *(ptr + 25), p++;
    for(int i = 28; i < 32; i++)
      *p = *(ptr + i), p++;
    ptr -= 32;
  }
  *p = 0;
  printfmt("filename is %s", name);
  return strdup(name);
}

struct vnode *fat32_build_long_file(char *beg, struct vnode *parent){
  char *ptr = beg;
  while(*(ptr + 11) & 0x0f) ptr += 32;
  struct SFN_entry fentry;
  read_sfn_entry(ptr, &fentry);
  struct vnode *rtn = newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
      newFat32LongFile(parse_long_filename(ptr - 32, beg), &fentry, parent));
}

int ignore_long_file(char *buffer, int i){
  while(i < 16){
    if((*buffer + i * 32 + 11) & 0x0f) i++;
    else break;
  }
  return i;
}

struct vnode *fat32_build_dir(struct SFN_entry *entry, struct vnode *parent){

  struct vnode *dir = newVnode(NULL, fat32_vop, fat32_fop, fat32_dop,
      newFat32Dir(entry, parent, NULL, NULL));

  struct vnode **iter = &(Fat32fd(dir)->child);

  char *buffer = (char*)kmalloc(sizeof(char) * 512);

  int N = entry_cluster_num(entry);
  unsigned cur_cluster_num = entry_cluster_num(entry);

  // qemu miss the bug v
  // if(!parent) puts("bug");

  unsigned prev_cluster_num =
    parent ? entry_cluster_num(&Fat32fd(parent)->entry) : 0;

  //printfmt("N = %d", N);

  while(N){
    //printfmt("dir N - %d", N);
    read_file_ctx(N, buffer);
    struct SFN_entry fentry;
    for(int i = 0; i < 16; i++){
      read_sfn_entry(buffer + i * 32, &fentry);
      if(*fentry.filename){
        if(*fentry.filename == 0xe5){
          // deleted file
          //puts("meet deleted file");
        }
        else if(fentry.attr & 0x10){
          if(entry_cluster_num(&fentry) == cur_cluster_num){
            // current directory '.'
            //puts("meet cur directory");
          }
          else if(entry_cluster_num(&fentry) == prev_cluster_num
              || entry_cluster_num(&fentry) == 0){
            // previous directory '..'
            //puts("meet prev directory");
          }
          else{
            *iter = fat32_build_dir(&fentry, dir);
            iter = &(Fat32fd(*iter)->next);
          }
        }
        else if((fentry.attr & 0x0f) == 0x0f){
          /* long file name */
          //puts("long file");
          //*iter = fat32_build_long_file(buffer + i * 32, dir);
          //i = ignore_long_file(buffer, i);
        }
        else{
          *iter = fat32_build_file(&fentry, dir);
          iter = &(Fat32fd(*iter)->next);
        }
      }
    }
    N = fat32_next_cluster(N, 0);
  }
  *iter = NULL;
  kfree(buffer);
  return dir;
}

/* http://www.c-jump.com/CIS24/Slides/FAT/F01_0180_sfn.htm */
struct vnode *fat32_build_root(){
  fat32_init();
  //printfmt("rooclust %d", bpb->bpbRootClust);
  struct SFN_entry *root_entry = /*{
                                   .filename = "/", .attr = 0x10,
                                   .reserved = 0, .create_ms = 0, .create_hms = 0,
                                   .date = 0, .access_date = 0,
                                   .start_hi = 0, .modify_time = 0, .modify_date = 0,
                                   .start_lo = bpb->bpbRootClust, .size = 0
                                   };*/ (struct SFN_entry *)kmalloc(sizeof(struct SFN_entry));
    root_entry->filename[0] = '/';
  root_entry->filename[1] = 0;
  root_entry->attr = 0x10;
  root_entry->reserved = 0;
  root_entry->create_ms = 0;
  root_entry->create_hms = 0;
  root_entry->date = 0;
  root_entry->access_date = 0;
  root_entry->modify_date = 0;
  root_entry->start_hi = 0;
  root_entry->modify_time = 0;
  root_entry->modify_date = 0;
  root_entry->start_lo = bpb->bpbRootClust;
  root_entry->size = 0;
  struct vnode *rtn = fat32_build_dir(root_entry, NULL);
  return rtn;
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

  if(subpath_of(".", component_name, upper_cmp)){
    return node->v_ops->lookup(
        node, target,
        component_name + strlen("."));
  }
  else if(subpath_of("..", component_name, upper_cmp)){
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
            component_name + strlen(".."));
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
    if(subpath_of(Fat32fd(child)->name, component_name, upper_cmp)){
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
    if(subpath_of(Fat32fd(*childptr)->name, component_name, upper_cmp)){
      return fat32_create(*childptr, target, component_name + strlen(Fat32fd(*childptr)->name));
    }
    childptr = &(Fat32fd(*childptr)->next);
  }
  if(exist_slash(component_name)) return 0;

  char code[11];

  encode_filename(component_name, code);

  //char *ctx = (char*)kmalloc(512 * sizeof(char));
  //struct SFN_entry *entry = &(Fat32fd(dir_node)->entry);
  //read_entry_ctx(entry, ctx);

  struct SFN_entry new_entry = {
    .attr = 0x20,
    .reserved = 0, .create_ms = 0, .create_hms = 0,
    .date = 0, .access_date = 0,
    .start_hi = 0, .modify_time = 0, .modify_date = 0,
    .start_lo = fat32_avail_cluster(FAT32_EOC), .size = 0
  };

  for(int i = 0; i < 11; i++)
    new_entry.filename[i] = code[i];

  printf("filename = %s ", component_name);
  show_entry_name(new_entry.filename);
  puts("");

  if(append_directory_entry(dir_node, &new_entry))
    return 0;

  *childptr =
    newVnode(NULL,
        dir_node->v_ops,
        dir_node->f_ops,
        dir_node->d_ops,
        newFat32File(&new_entry, dir_node));

  *target = *childptr;
  return 1;
}

int fat32_write(struct file *file, const void *buf, size_t len){
  struct SFN_entry *entry = &(Fat32fd(file->vnode)->entry);
  unsigned int size = file->f_pos + len;

  if(size > 512){
    puts("not support file larger then 512 bytes currently");
    return 1;
  }

  char *ctx = (char*)kmalloc(512 * sizeof(char));
  char *pos = ctx + file->f_pos, *ptr = (char*)buf;

  /* update the meta data */
  entry->size = MAX(entry->size, size);
  if(update_file_metadata(Fat32fd(file->vnode)->parent, entry))
    return 1;

  /* write the content */
  read_entry_ctx(entry, ctx);
  while(len--){
    *pos = *ptr;
    pos++, ptr++;
  }
  *pos = EOF;
  write_entry_ctx(entry, ctx);
  kfree(ctx);
  return 0;
}

int fat32_read(struct file *file, void *buf, size_t len){
  char ctx[512];
  struct SFN_entry *entry = &(Fat32fd(file->vnode)->entry);
  read_entry_ctx(entry, ctx);
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
#if 0
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
#endif
  struct SFN_entry new_entry = {
    .attr = 0x10,
    .reserved = 0, .create_ms = 0, .create_hms = 0,
    .date = 0, .access_date = 0,
    .start_hi = 0, .modify_time = 0, .modify_date = 0,
    .start_lo = fat32_avail_cluster(FAT32_EOC), .size = 0
  };

  char code[11];
  encode_filename(pathname, code);

  for(int i = 0; i < 11; i++)
    new_entry.filename[i] = code[i];

  if(append_directory_entry(node, &new_entry))
    return 0;

  printf("directory = %s ", pathname);
  for(int i = 0; i < 11; i++)
    printf("%c", new_entry.filename[i]);
  puts("");

#if 0
  if(node->v_ops->lookup(node, &target, p)){
    target = move_mount_root(target);
#endif
    Fat32fd(node)->child =  newVnode(
        NULL, fat32_vop, fat32_fop, fat32_dop,
        newFat32Dir(
          &new_entry, node, NULL, Fat32fd(node)->child));

    for(int i = 0; i < 11; i++)
      new_entry.filename[i] = ' ';
    new_entry.filename[0] = '.';

    if(append_directory_entry(Fat32fd(node)->child, &new_entry))
      return 0;

    new_entry.filename[1] = '.';
    new_entry.start_lo = 0x00;
    if(append_directory_entry(Fat32fd(node)->child, &new_entry))
      return 0;
#if 0
  }
#endif
  return 0;
}

int fat32_chdir(struct vnode *node, const char *pathname){
  struct vnode *target;
  if(node->v_ops->lookup(node, &target, pathname)){
    target = move_mount_root(target);
    printfmt("pwd <%x>", target);
    current_task->pwd = target;
    return 0;
  } else puts("[fat32] no such file or directory");
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
