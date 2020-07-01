#include "fat32.h"
#include "sdlib.h"
#include "printf.h"
#include "vfs.h"
#include "string.h"
#include "allocator.h"

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof (unsigned int))
#define CHAIN_EOF 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof (Fat32Dentry))

static VNodeOperations _v_ops = { 
    .create = fat32_create,
    .lookup = fat32_lookup,
    .ls = fat32_ls,
    .mkdir = fat32_mkdir
};
static FileOperations _f_ops = { 
    .read = fat32_read,
    .write = fat32_write
};
static VNodeOperations *v_ops = &_v_ops;
static FileOperations *f_ops = &_f_ops;

void print_block(int blockIndex) {
    char buf[512] = {0};
    readblock(blockIndex, buf);
    for (int i = 0; i < 512; i++) {
        if (i % 16 == 0)  printf("%08x: ", blockIndex*512+i);
        printf("%02x", buf[i]);
        if (i % 2 == 1)   printf(" ");
        if (i % 16 == 15) printf("\n");
    }
}

void fat32fs_vnode_init(VNode *vnode) {
    vnode->v_ops = v_ops;
    vnode->f_ops = f_ops;
}

Fat32Dentry *find_fat32_dentry(Fat32Node *node) {
    Fat32Dentry *dentry = malloc(sizeof(Fat32Dentry));
    Fat32Dentry dirs[DIR_LEN];

    int blockIndex = 
        node->info.lba +
        node->info.reservedCount +
        node->info.fatNum * node->info.sectorsPerFat +
        node->dentry -
        node->info.clusterNumOfRoot;
    
    readblock (blockIndex, dirs);
    *dentry = dirs[node->dirIndex];
    return dentry;
}

int set_fat32_dentry(Fat32Node *node, Fat32Dentry *dentry)
{
    Fat32Dentry dirs[DIR_LEN];
    unsigned int offset;
    offset = node->info.lba + node->info.clusterNumOfRoot;
    offset += node->info.fatNum * node->info.sectorsPerFat;
    readblock (offset + node->dentry - node->info.clusterNumOfRoot, dirs);
    dirs[node->dirIndex] = *dentry;
    writeblock (offset + node->dentry - node->info.clusterNumOfRoot, dirs);
    return 0;
}

int fat32_filename_equal(Fat32Dentry *dentry, char *filename) {
    int name_ind, ext_ind;
    const char *extname;
    for (name_ind = 0; name_ind < 8; ++name_ind) {
        if (filename[name_ind] == '\0')
            return dentry->name[name_ind] == '\x20';
        if (filename[name_ind] == '.') {
            if (dentry->name[name_ind] != '\x20') return 0;
            break;
        }
        if (filename[name_ind] != dentry->name[name_ind])
            return 0;
    }
    if (filename[name_ind] == '\0')
        return 1;
    extname = &filename[name_ind + 1];
    for (ext_ind = 0; ext_ind < 3; ++ext_ind) {
        if (extname[ext_ind] == '\0')
            return dentry->ext[ext_ind] != '\x20';
        if (extname[ext_ind] != dentry->ext[ext_ind])
            return 0;
    }
    return 1;
    if (dentry->name[name_ind] != '\x20')
        return 0;
    if (filename[name_ind] == '\0' && dentry->name[name_ind] == '\x20'
        && dentry->ext[0] == '\x20')
        return 1;
    ++name_ind;
    for (ext_ind = 0; ext_ind < 3; ++ext_ind) {
        if (filename[name_ind + ext_ind] == '\0') break;
    }
}

unsigned int find_fat32_cluster_index(Fat32Info *info, unsigned int index) {
    unsigned int offset;
    unsigned int fat[CHAIN_LEN];

    // find FAT
    offset = info->lba + info->reservedCount;
    offset += index / CHAIN_LEN;
    readblock (offset, fat);
    return fat[index % CHAIN_LEN];
}

void fat32_setup_mount(FileSystem *fs, Mount *mount) {
    sd_init();
    char buf[512] = {0};
    PartitionEntry *part1;
    BootSector *fat32;
    Fat32Node *init_node = (Fat32Node *) malloc(sizeof(Fat32Node));
    VNode *vnode = (VNode *) malloc(sizeof(VNode));
    readblock(0, buf);
    part1 = (void *) buf;
    init_node->info.lba = part1->lba;
    init_node->info.size = part1->size;

    readblock(init_node->info.lba, buf);
    fat32 = (void *) buf;
    init_node->info.clusterNumOfRoot = fat32->clusterNumOfRoot;
    init_node->info.reservedCount = fat32->reservedCount;
    init_node->info.fatNum = fat32->fatNum;
    init_node->info.sectorsPerFat = fat32->sectorsPerFat;
    init_node->clusterIndex = fat32->clusterNumOfRoot;
    init_node->dentry = 0;
    init_node->dirIndex = 0;
    
    fat32fs_vnode_init(vnode);
    vnode->internal = init_node;

    mount->fs = fs;
	mount->root = vnode;
}

int fat32_write(File *file, const void *buf, int len) {
    printf("[write]\n");
    Fat32Node *fnode = file->vnode->internal;
    Fat32Dentry *dir = find_fat32_dentry(fnode);

    int clusterIndex = fnode->clusterIndex;
    int offset = fnode->info.lba + fnode->info.reservedCount;
    offset += fnode->info.fatNum * fnode->info.sectorsPerFat;

    char data[BLOCK_SIZE];
    int size = dir->size;
    int blocks = 0;

    if (file->f_pos + len > size) {
        dir->size = file->f_pos + len;
        set_fat32_dentry(fnode, dir);
    }

    while ((clusterIndex & CHAIN_EOF) != CHAIN_EOF) {
        readblock(offset + clusterIndex - fnode->info.clusterNumOfRoot, data);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            data[i] = ((char *)buf)[blocks * BLOCK_SIZE + i];
            if (--len == 0) break;
        }
        blocks ++;
        writeblock (offset + clusterIndex - fnode->info.clusterNumOfRoot, data);
        clusterIndex = find_fat32_cluster_index(&fnode->info, clusterIndex);
        if (len == 0) break;
    }
    return 1;
}

int fat32_read(File *file, void *buf, int len) {
    printf("[read]\n");
    Fat32Node *fnode = file->vnode->internal;
    Fat32Dentry *dir = find_fat32_dentry(fnode);
    int clusterIndex = fnode->clusterIndex;
    int offset = fnode->info.lba + fnode->info.reservedCount;
    offset += fnode->info.fatNum * fnode->info.sectorsPerFat;

    char data[BLOCK_SIZE];
    int blocks = 0;

    while ((clusterIndex & CHAIN_EOF) != CHAIN_EOF) {
        readblock(offset + clusterIndex - fnode->info.clusterNumOfRoot, data);
        for (int i = 0; i < BLOCK_SIZE; i++) {
            ((char *)buf)[blocks * BLOCK_SIZE + i] = data[i];
            if (--len == 0) break;
        }
        blocks ++;
        clusterIndex = find_fat32_cluster_index(&fnode->info, clusterIndex);
        if (len == 0) break;
    }
    // print_block(offset + clusterIndex - fnode->info.clusterNumOfRoot);
    return 1;
}

int fat32_ls(VNode *node) {
    printf("[ls]: \n");
    Fat32Dentry dirs[DIR_LEN];
    Fat32Node *fat32_node = node->internal;

    int blockIndex = 
        fat32_node->info.lba +
        fat32_node->info.reservedCount +
        fat32_node->info.fatNum * fat32_node->info.sectorsPerFat +
        fat32_node->clusterIndex -
        fat32_node->info.clusterNumOfRoot;
    readblock(blockIndex, dirs);

    for (int i = 0; i < DIR_LEN; i++) {
        if (!str_equal( dirs[i].name, "")) {
            printf(" - %s\n", dirs[i].name);
        }
    }
    return 1;
}

int fat32_lookup(VNode *dir_node, VNode **target, const char *component_name) {
    printf("[lookup]\n");
    Fat32Node *fat32_node = dir_node->internal;
    Fat32Dentry dirs[DIR_LEN];

    int blockIndex = 
        fat32_node->info.lba +
        fat32_node->info.reservedCount +
        fat32_node->info.fatNum * fat32_node->info.sectorsPerFat +
        fat32_node->clusterIndex -
        fat32_node->info.clusterNumOfRoot;
    readblock(blockIndex, dirs);
    
    for (int i = 0; i < DIR_LEN; i++) {
        if (fat32_filename_equal(&dirs[i], component_name)) {
            VNode *vnode = (VNode *) malloc(sizeof(VNode));
            Fat32Node *fnode = (Fat32Node *) malloc(sizeof(Fat32Node));
            fnode->clusterIndex = (dirs[i].startHi << 16) + dirs[i].startLo;
            fnode->dentry = fat32_node->clusterIndex;
            fnode->dirIndex = i;
            fat32fs_vnode_init(vnode);
            fnode->info = fat32_node->info;
            vnode->internal = fnode;
            *target = vnode;
            return 1;
        }
    }
    return 0;
}

int fat32_create(VNode *dir_node, VNode **target, const char *component_name) {
    return 1;
}

int fat32_mkdir(VNode *dir_node, VNode **target, const char *component_name) {
    return 1;
}