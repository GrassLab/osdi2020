#include "sd.h"
#include "uart.h"
#include "string.h"

// get the end of bss segment from linker
extern unsigned char _end;

static unsigned int partitionlba = 0;

typedef struct {
    char            code_area[446]; // 0x0 ~ 0x1BD
    char            partition_table[64]; // 0x1BE ~ 0x1FD
    char            signature[2]; // 0x1FE ~ 0x1FF
} __attribute__((packed)) mbr_t;

typedef struct {
    char            boot_indicator;
    char            CHS_start[3];
    char            descriptor;
    char            CHS_end[3];
    unsigned int    start_sector;
    unsigned int    partition_size;
} __attribute__((packed)) partition_t;

typedef struct {
    char            jmp[3];
    char            oem[8]; 
    unsigned char   bps[2];   // byte per sector  
    unsigned char   min_sector_per_unit;   // min sector per unit
    unsigned short  reserve_sector;  // reserve sector
    unsigned char   fat_num;
    unsigned char   attr1[0xf];
    unsigned char   fs_sector[4];
    unsigned int    fat_sector;
    unsigned char   attr2[4];
    unsigned int    root_fat_number;
    unsigned char   attr3[0x22];
    unsigned char   fs_format[8];
} __attribute__((packed)) bpb_t;

typedef struct {
     unsigned int cluster[0x80];
} __attribute__((packed)) fat_t;


// directory entry structure
typedef struct {
    char            name[8];
    char            ext[3];
    char            attr1[9];
    unsigned short  ch;
    char            attr2[4];
    unsigned short  cl;
    unsigned int    size;
} __attribute__((packed)) fatdir_t;

typedef struct {
    char name[9];
    char ext[4];
    fatdir_t *dentry;
    unsigned long fat_begin;
    unsigned int file_content_sector
} file_t;

typedef struct {
    int dentry_num;
    char mbr[512];
    partition_t *partition_entry[4];
    char partition1[512];
    unsigned long partition1_sector;
    char root_dir[512];
    unsigned long root_dir_sector;
    file_t dentry_obj[0x10];
    unsigned long fat1_sector;
} fat_manager;

fat_manager FAT_MANAGER;

void sector_debug(char *buf){
    // debugging
    for (int i=0; i<512; i++) {
        printf("%x ", buf[i]);
        if ((i % 0x10) == 0xf) {
            printf("\n");
        }
    }
}

int fat_get_mbr()
{
    mbr_t *mbr = (mbr_t*)&FAT_MANAGER.mbr;
    readblock(0, &FAT_MANAGER.mbr);

    sector_debug((char*)&FAT_MANAGER.mbr);


    partition_t *par;
    int i;
    for (i=0; i<4; i++) {
        par = (partition_t*)mbr->partition_table + i;
        if (par->boot_indicator == 0x80) {
            if (par->descriptor == 0xb) {
                printf("partition%d: FAT32\n", i+1);
            } else {
                printf("partition%d: UNKNOWN\n", i+1);
            }
            // printf("start_sector: %x\n", par->boot_indicator);
            // printf("start_sector: %x\n", par->start_sector);
            FAT_MANAGER.partition_entry[i] = par;
        } else {
            break;
        }
    }
    if (i == 0) {
        return -1;
    }
    printf("start_sector: %d\n", FAT_MANAGER.partition_entry[0]->start_sector);
    return 1;
}

int fat_get_partition()
{
    bpb_t *bpb = (bpb_t*)&FAT_MANAGER.partition1;
    FAT_MANAGER.partition1_sector = 2048;
    readblock(FAT_MANAGER.partition1_sector, &FAT_MANAGER.partition1);
    // printf("%s\n", bpb->fs_format);
    FAT_MANAGER.fat1_sector = FAT_MANAGER.partition1_sector + bpb->reserve_sector;
    FAT_MANAGER.root_dir_sector = FAT_MANAGER.fat1_sector + bpb->fat_sector*2;

    printf("bpb->reserve_sector: %d\n", bpb->reserve_sector);
    printf("bpb->fat_sector: %d\n", bpb->fat_sector);
    printf("fat1_sector: %d\n", FAT_MANAGER.fat1_sector);
    printf("root_dir_sector: %d\n", FAT_MANAGER.root_dir_sector);


    return 1;
}

int fat_get_root_dir()
{
    readblock(FAT_MANAGER.root_dir_sector, &FAT_MANAGER.root_dir); // get root_dir's sector
    int entry_num = 0;
    fatdir_t *dir;
    for (;;) {
        dir = (fatdir_t*)&FAT_MANAGER.root_dir[entry_num];
        file_t *file = &FAT_MANAGER.dentry_obj[FAT_MANAGER.dentry_num];
        file->dentry = dir;
        if ((dir->name[0] == 0) & (dir->name[1] == 0)) {
            return 0;
        } else {
            // get filename 
            // strncpy(&file->name, dir->name, 8);
            char* dst = &file->name;
            char* src = dir->name;
            int i;
            for (i=0; i<8; i++) {
                if (*src == 0x20) {
                    break;
                }
                *dst++ = *src++;
            }
            file->name[i] = '\0';
            strncpy(&file->ext, dir->ext, 3);
            file->ext[3] = '\0';
            printf("id: %d,%s.%s\n", FAT_MANAGER.dentry_num, file->name, file->ext);
            // get fat start cluster
            file->fat_begin = ((dir->ch)<<16) + dir->cl;
            file->file_content_sector = FAT_MANAGER.root_dir_sector + file->fat_begin - 2;
        }
        FAT_MANAGER.dentry_num++;
        entry_num += 0x20;
    }

}

int fat_setup_mount() 
{
    /*
    1. Parse the metadata on the SD card.
    2. Create a kernel object to store the metadata in memory.
    3. Get the root directory cluster number and create a FAT32’s root directory object.
    */
    printf("+++ fat setup mount +++\n");
    // fat_get_mbr();
    fat_get_partition();
    // fat_get_fat_table();
    fat_get_root_dir();
    return 0;
}

void fat_get_fat_table()
{
    fat_t *fat_entry = (unsigned int*)&_end;
    readblock(FAT_MANAGER.fat1_sector, &_end); // get fat1's sector
    printf("fat_entry[0]: %x\n", &fat_entry->cluster[0]);
    printf("fat_entry[1]: %x\n", &fat_entry->cluster[1]);
    printf("fat_entry[2]: %x\n", &fat_entry->cluster[2]);
    printf("fat_entry[3]: %x\n", &fat_entry->cluster[3]);
}

int fat_lookup(const char* name, const char* ext)
{
    /*
    1. Get the cluster of the directory and calculate its block index.
    2. Read the first block of the cluster by the readblock()
    3. Traverse the directory entries and compare the component name with filename + extension name to find the file.
    4. You can get the first cluster of the file in the directory entry.
    */
    printf("+++ fat lookup file +++\n");
    file_t *file;
    for (int i=0; i<FAT_MANAGER.dentry_num; i++) {
        file = &FAT_MANAGER.dentry_obj[i];
        printf("%s\n", file->name);
        if ((strcmp(name, file->name)==0)
            & (strcmp(ext, file->ext)==0)) {
            printf("found it!!\n");
            return i;
        }
    }
    printf("not found QQ\n");
    return -1;
}

void fat_read_file()
{
    printf("+++ fat read file +++\n");
    file_t *file = &FAT_MANAGER.dentry_obj[13];
    fatdir_t *dir = FAT_MANAGER.dentry_obj[13].dentry;
    printf("name: %s\n", file->name);
    printf("fat start cluster: %x\n", file->fat_begin);
    printf("file content sector: %d\n", file->file_content_sector);
    
    for (int i=0; i<2; i++) {
        readblock(file->file_content_sector+i, &_end);
        sector_debug((char*)&_end);
    }
    return;
}

char write_data[512] = "AA";

void fat_write_file()
{
    printf("+++ fat write file +++\n");
    file_t *file = &FAT_MANAGER.dentry_obj[13];
    fatdir_t *dir = FAT_MANAGER.dentry_obj[13].dentry;
    printf("name: %s\n", file->name);
    printf("fat start cluster: %x\n", file->fat_begin);
    printf("file content sector: %d\n", file->file_content_sector);
    printf("file size: %x\n", file->dentry->size);
    
    
    writeblock(file->file_content_sector, &write_data);
    file->dentry->size = 0x2;
    writeblock(FAT_MANAGER.root_dir_sector, &FAT_MANAGER.root_dir);
    
    return;
}

void fat_create(const char* component_name)
{
    /*
    1. Find an empty entry in the FAT table.
    2. Find an empty directory entry in the target directory.
    3. Set them to proper values.
    */
    return;
}

void fat_read();
void fat_write();
void fat_open();
void fat_close();


