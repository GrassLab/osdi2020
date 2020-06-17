#include "vfs.h"
#include "slab.h"
#include "mm.h"
#include "printf.h"
#include "sched.h"

static struct mount *rootfs_mount;
static struct directory *current_dir; 

int strcmp_eq(char *str1, char *str2) {
    while (1) {
        if (*str1 != *str2) {
            return 0;
        }
        if (*str1 == '\0' && *str2 == '\0') {
            return 1;
        }
        str1++;
        str2++;
    }
}

int strcpy(char *dest, char *source) {
    int index = 0;
    while(*source != '\0' && *source != '/') {
        *dest = *source;
        dest++;
        source++;
        index++;
    }
    if (*source == '/') {
        *dest = '\0';
        return index+1;
    }
    *dest = '\0';
    return 0;
}

struct vnode* entry_file_find(struct directory *dir, const char *comp_name) {
    if(dir->head == 0) return 0;
    struct direntry *iter_entry = dir->head;
    if(iter_entry == 0) return 0;
    while(iter_entry != 0) {
        if((iter_entry->entry->type == REG_FILE) && strcmp_eq(((struct direntry*)iter_entry)->name, comp_name)) {
            return iter_entry->entry;
        }
        iter_entry = iter_entry->next;
    }
    return 0;
}

struct vnode* entry_dir_find(struct directory *dir, char *comp_name) {
    if(dir->head == 0) return 0;
    struct direntry *iter_entry = dir->head;
    if(iter_entry == 0) return 0;
    while(iter_entry != 0) {
        if((iter_entry->entry->type == REG_DIR) && strcmp_eq(((struct direntry*)iter_entry)->name, comp_name)) {
            return iter_entry->entry;
        }
        iter_entry = iter_entry->next;
    }
    return 0;
}

int setup_mount(struct filesystem* fs, struct mount* mount) {
    mount->fs = fs;
    return 1;
}

void create_file_vnode(struct file *create_file) {
    struct vnode *create_vnode     = obj_allocate(sizeof(struct vnode));
    create_vnode->type             = REG_FILE;
    create_vnode->internal         = (struct block*)get_free_pages(1);
    *(int *)create_vnode->internal = 0; // set the file size
    create_file->f_pos     = 0;
    create_file->vnode = create_vnode;
    return;
}

struct vnode *create_dir_vnode() {
    struct vnode *create_vnode     = obj_allocate(sizeof(struct vnode));
    create_vnode->type             = REG_DIR;
    create_vnode->internal         = obj_allocate(sizeof(struct directory));
    return create_vnode;
}

void iterate_dir(struct directory *dir) {
    struct direntry *iter_entry = dir->head;
    while(iter_entry != 0) {
        printf("%s\r\n", iter_entry->name);
        iter_entry = iter_entry->next;
    }
}

void insert_vnode(struct directory *dir, struct vnode *in_vnode, char *name) {
    struct direntry *create_entry = obj_allocate(sizeof(struct direntry));
    create_entry->name = obj_allocate(sizeof(char) * 10);
    strcpy(create_entry->name, name);
    create_entry->entry = in_vnode;
    if(dir->head == 0) {
        dir->head = create_entry;
        dir->head->next = 0;
        dir->tail = dir->head;
    }
    else {
        dir->tail->next = create_entry;
        dir->tail = dir->tail->next;
        dir->tail->next = 0;
    }
}

struct directory *find_dir(const char* pathname) {
    if(pathname == 0) return 0;
    char path_name_buff[20];
    struct directory *ret_dir;
    if(pathname[0] == '/') {
        // root dir
        ret_dir = (struct directory *)rootfs_mount->root->internal;
        if(pathname[1] == '\0') return ret_dir;
        int index = 1;
        index = strcpy(path_name_buff, &pathname[index]);
        while(1) {
            struct vnode *find_vnode = entry_dir_find(ret_dir, path_name_buff);
            if (find_vnode == 0) {
                return ret_dir;
            }
            ret_dir = (struct directory *)find_vnode->internal;
            index = strcpy(path_name_buff, &pathname[index]);
        }
    }
    else {
        // current dir
        return current_dir;
    }
}

void register_filesystem(struct filesystem* fs, char *fs_name) {
  // register the file system to the kernel.
  // you can also initialize memory pool of the file system here.
    fs->name = obj_allocate(sizeof(char) * 10);
    strcpy(fs->name, fs_name);
    fs->setup_mount = setup_mount;
}

void init_root_filesystem()
{
    // init vnode
    // mount the vnode 
    struct vnode *root_vnode   = obj_allocate(sizeof(struct vnode));
    // root vnode is directory 
    root_vnode->type           = REG_DIR;
    root_vnode->internal       = obj_allocate(sizeof(struct directory));
    current_dir = (struct directory*)root_vnode->internal;
    struct filesystem *root_fs = obj_allocate(sizeof(struct filesystem));
    // register file system
    register_filesystem(root_fs, "root_fs");

    rootfs_mount = obj_allocate(sizeof(struct mount));
    rootfs_mount->root = root_vnode;
    int setup = root_fs->setup_mount(root_fs, rootfs_mount);
    if(setup == 0) printf("Error register root fs\r\n");
}

struct file* vfs_open(const char* pathname, int flags) { 
    //only regular file
    struct vnode *root_vnode = rootfs_mount->root;
    struct file  *ret_file   = obj_allocate(sizeof(struct file)); // file descriptor
    if(root_vnode == 0) return 0;
    if((flags & REG_DIR) > 0) {
        struct directory *start_dir = find_dir(pathname);
        iterate_dir(start_dir);
        return 0;
    }
    if((flags & REG_FILE) > 0) {
        // find the vnode
        struct directory *start_dir = find_dir(pathname);
        char dir_name[20];
        int index = 0;
        struct vnode *find_vnode = entry_file_find(start_dir, pathname); 

        if (find_vnode == 0 && (flags & O_CREAT) == 0) {
            // doesn't find the vnode
            return 0;
        }
        else if (find_vnode == 0 && (flags & O_CREAT) > 0) {
            create_file_vnode(ret_file);
            // insert vnode to dir
            insert_vnode(start_dir, ret_file->vnode, pathname);
        }
        else{
            ret_file->vnode = find_vnode;
            ret_file->f_pos = 0;
        }
        return ret_file;
    }
    return 0;
}

int vfs_close(unsigned long file) {
    slab_put_obj(file);
    return 1;
}

int vfs_write(struct file* file, const void* buf, int len) {
    // get the offset
    int f_pos = file->f_pos;
    int *file_size = (int *)(file->vnode->internal);
    memcpy((unsigned long)buf, (unsigned long)file->vnode->internal + sizeof(int) + f_pos, len);
    file->f_pos += len;
    if(*file_size < file->f_pos) {
        *file_size = file->f_pos;
    }
    return 1;
}

int vfs_read(struct file* file, void* buf, int len) {
    int f_pos = file->f_pos;
    int *file_size = (int *)(file->vnode->internal);
    if((f_pos + len) >= *file_size) {
        len = *file_size - f_pos;
    }
    memcpy((unsigned long)file->vnode->internal + sizeof(int) + f_pos, (unsigned long)buf, len);
    file->f_pos += len;
    return len;
}

int mkdir(const char* pathname) {
    struct directory *start_dir = (struct directory *)find_dir(pathname);
    // allocate vnode
    struct vnode *new_vnode  = create_dir_vnode();
    insert_vnode(start_dir, new_vnode, pathname);
    return 1;
}


int user_open(const char* pathname, int flags) {
    // find a empty file descriptor in user task
    int i;
    struct file *file_descriptor;
    for(i = 0 ; i < MAX_FILE_NUM ; i++) {
        if(current->file_desp[i] == 0) {
            // allocate file descriptor
            current->file_desp[i] = vfs_open(pathname, flags);
            file_descriptor = current->file_desp[i];
            break;
        }
    }
    if(file_descriptor == 0) return -1;
    return i; 
}

int user_read(int file_index, void* buf, int len) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    return vfs_read(file_descriptor, buf, len);
}

void user_write(int file_index, const void* buf, int len) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    return vfs_write(file_descriptor, buf, len);
}

void user_close(int file_index) {
    if(file_index >= MAX_FILE_NUM) return -1;
    struct file *file_descriptor;
    file_descriptor = current->file_desp[file_index];
    if(file_descriptor == 0) return -1;
    vfs_close(file_descriptor);
    current->file_desp[file_index] = 0;

    return;
}

