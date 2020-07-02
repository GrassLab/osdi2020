#include "fat.h"


#include "sdcard.h"
#include "mm.h"
#include "printf.h"
#include "vfs.h"
#include "lib/string.h"

char file_name[] = "BOOTCODE.BIN";

struct vnode_operations fat_v_ops = {lookup_fat, create_fat};
struct file_operations fat_f_ops = {read_fat, write_fat};

partition_entry_t *entry1;
boot_sector_t *boot_sec;
int data_sec_idx;

int fat_getpartition(){
    sd_init();
    // read the MBR, find the boot sector
    unsigned char mbr[BLOCK_SIZE];
    readblock(0, mbr); 

    // check boot magic number
    if(mbr[0x1FE]!=0x55 || mbr[0x1FF]!=0xAA) {
        printf("[fat getpartition] Bad magic in MBR\n");
        return 0;
    }
    // load first partition entry
    entry1 = kmalloc(sizeof(partition_entry_t));
    memcpy(entry1, mbr+FIRST_PART_ENTRY_OFFSET, sizeof(partition_entry_t));

    // check partition type
    if(entry1->partition_type != PARTITION_TYPE_FAT32){
        printf("[fat getpartition] Wrong partition type %d\n", entry1->partition_type);
        return 0;
    }
    
    printf("### FAT32 with CHS addressing\n");
    printf("### Block index: %d\n", entry1->starting_sector);
    printf("### Partition size: %d (sectors)\n", entry1->nr_sector);

    //  boot sector of FAT32 partition’s block
    unsigned char partition_block[BLOCK_SIZE];
    readblock(entry1->starting_sector, partition_block);
    
    boot_sec = kmalloc(sizeof(boot_sec));
    memcpy(boot_sec, partition_block, sizeof(boot_sector_t));


    data_sec_idx = entry1->starting_sector + boot_sec->nr_reserved_sectors + (boot_sec->nr_fat32_table * boot_sec->nr_sector_fat32);
    return 1;
}

void setup_fs_fat(struct filesystem *fs)
{
    fs->name = "fat";
    fs->setup_mount = setup_mount_fat;
}

struct vnode *create_fat_vnode(int type, unsigned int first_cluster, unsigned int size)
{
    struct vnode *vnode = kmalloc(sizeof(struct vnode));
    vnode->mount = 0;//NULL
    vnode->v_ops = &fat_v_ops;
    vnode->f_ops = &fat_f_ops;
    vnode->cache = 0;//NULL
    vnode->type = type;
    vnode->internal = kmalloc(sizeof(struct fat_internal));
    ((struct fat_internal *)vnode->internal)->first_cluster = first_cluster;
    ((struct fat_internal *)vnode->internal)->size = size;

    return vnode;
}

int setup_mount_fat(struct filesystem *fs, struct mount *mnt)
{
    fat_getpartition();
    struct vnode *vnode = create_fat_vnode(VNODE_TYPE_DIR, 0, 0);

    mnt->fs= fs;
    mnt->root = vnode;

    return 0;
}

int lookup_fat(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    *target = 0;

    vnode->cache = kmalloc(sizeof(struct vnode_cache));
    memset(vnode->cache, 0U, sizeof(struct vnode_cache));

    struct dentry *dentries = vnode->cache->dentries;
    struct fat_internal* internal = vnode->internal;
    
    unsigned char sector[BLOCK_SIZE];
    struct fat_dir *dir = (struct fat_dir *)sector;
    // we assume all object only use 1 block, and need not to checkout fat
    readblock(data_sec_idx + internal->first_cluster, sector);
    // readblock(data_sec_idx + internal->first_cluster*boot_sec->sector_per_cluster, sector);
    for(int cnt=0; dir->name[0]!='\0'; dir++){
        if(dir->name[0]==0xE5 || dir->attr[0]==0xF) //0x00 Entry never used, 0xe5 File is deleted
            continue;
        //cache vnode
        int type = dir->name[0]=='.' ? VNODE_TYPE_DIR : VNODE_TYPE_REG;
        unsigned int first_cluster = (((unsigned int)dir->cluster_high)<<16 | dir->cluster_low)-1;
        dentries[cnt].vnode = create_fat_vnode(type, first_cluster, dir->size);
        //cache name
        int i;
        for(i=0; i<8; i++){
            if(dir->name[i] == ' ')
                break;
            dentries[cnt].name[i] = dir->name[i];
        }
        dentries[cnt].name[i] = '.';
        dentries[cnt].name[i+1] = dir->ext[0];
        dentries[cnt].name[i+2] = dir->ext[1];
        dentries[cnt].name[i+3] = dir->ext[2];
        dentries[cnt].name[i+4] = '\0';

        if(strcmp(dentries[cnt].name, component_name) == 0)
            *target = dentries[cnt].vnode;

        cnt++;
    }
    return 0;
}

//TODO: allocate sector in device or implement dirty fn
int create_fat(struct vnode *vnode, struct vnode **target, const char *component_name)
{
    if(vnode->type != VNODE_TYPE_DIR){
        printf("[create fat] Parent-node is not a directory!\n");
        *target = 0;
        return -1;
    }
    struct dentry* dentries = vnode->cache->dentries;
    int free_idx = 0;
    while(free_idx<NR_CHILD && dentries[free_idx].vnode!=0){
        free_idx++;
    }
    if(free_idx == NR_CHILD){
        printf("[create fat] Direcotry can only has %d child entries!\n", NR_CHILD);
        *target = 0;
        return -1;
    }
    // create new node
    *target = create_fat_vnode(VNODE_TYPE_REG, 0, 0);// TODO
    (*target)->cache = kmalloc(sizeof(struct vnode_cache));
    (*target)->cache->regbuf[0] = EOF;
    // add link to parent direcory
    strncpy(dentries[free_idx].name, component_name, DNAME_LEN);
    dentries[free_idx].vnode = *target;
    printf("[create fat] file name: %s. Create success!\n", component_name);
    return 0;
}

//TODO fat EOF
int read_fat(struct file *file, void *buf, unsigned len)
{
    file->vnode->cache = kmalloc(sizeof(struct vnode_cache));
    memset(file->vnode->cache, 0U, sizeof(struct vnode_cache));
    struct fat_internal* internal = file->vnode->internal;
    // cache regular file
    char *_regbuf = (char *)file->vnode->cache->regbuf;
    // we assume all object only use 1 block, and need not to checkout fat
    readblock(data_sec_idx + internal->first_cluster, _regbuf);
    // readblock(data_sec_idx + internal->first_cluster*boot_sec->sector_per_cluster, _regbuf);
    // _regbuf[internal->size]=EOF;
    file->f_pos = 0;

    char *_buf = (char *)buf;
    unsigned cnt;
    // for(cnt=0; (cnt<len && _regbuf[cnt] != EOF); cnt++){
    for(cnt=0; (cnt<len && cnt != (internal->size<REGBUFF_SIZE ?internal->size : REGBUFF_SIZE)); cnt++){
        _buf[cnt] = _regbuf[cnt];
    }
    _buf[cnt] = '\0';
    printf("[read fat] %d byte(s) read. f_pos %d -> %d\n", cnt, file->f_pos, file->f_pos+cnt);
    file->f_pos += cnt;
    return cnt;
}

// TODO dirty_fat
int write_fat(struct file *file, const void *buf, unsigned len)
{
    if(file->vnode->cache == 0){
        printf("[write fat] cache is empty, nothing to write!\n");
        return 0;
    }

    // printf("%dxxxxx\n%dxxxxx\n%dxxxxx\n", boot_sec->sector_per_cluster, entry1->starting_sector , boot_sec->nr_reserved_sectors);
    struct fat_internal* internal = file->vnode->internal;
    char *_regbuf = (char *)file->vnode->cache->regbuf;
    // we assume all object only use 1 block, and need not to checkout fat
    // writeblock(data_sec_idx + internal->first_cluster*boot_sec->sector_per_cluster, _regbuf);
    writeblock(data_sec_idx + internal->first_cluster, _regbuf);

    unsigned cnt = (len<BLOCK_SIZE) ? len : BLOCK_SIZE;

    unsigned char sector[BLOCK_SIZE];
    struct fat_dir *dir = (struct fat_dir *)sector;
    // we assume all object only use 1 block, and need not to checkout fat

    // readblock(data_sec_idx + internal->first_cluster*boot_sec->sector_per_cluster, sector);
    readblock(data_sec_idx + internal->first_cluster, sector);
    for(int cnt=0; dir->name[0]!='\0'; dir++){
        if(dir->name[0]==0xE5 || dir->attr[0]==0xF) //0x00 Entry never used, 0xe5 File is deleted
            continue;
        //cache name
        char _name[13]={0};
        int i;
        for(i=0; i<8; i++){
            if(dir->name[i] == ' ')
                break;
            _name[i] = dir->name[i];
        }
        _name[i] = '.';
        _name[i+1] = dir->ext[0];
        _name[i+2] = dir->ext[1];
        _name[i+3] = dir->ext[2];
        _name[i+4] = '\0';

        if(strcmp(_name, file_name) == 0)
            dir->size = cnt;

        cnt++;
    }
    // update size
    writeblock(data_sec_idx + internal->first_cluster, sector);


    printf("[write fat] cache write backed (%d bytes)\n", cnt);
    return cnt;
}