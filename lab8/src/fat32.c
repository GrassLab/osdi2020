#include "uart.h"
#include "string.h"
#include "buddy.h"
#include "vfs.h"
#include "fat32.h"
#include "mm.h"
unsigned int root_sec_index = 0;

int fat32_partition(){
    // read the MBR, find the boot sector
    unsigned char mbr[BLOCK_SIZE];
    readblock(0, mbr); 
    
    // signature
    if(mbr[510] != 0x55 || mbr[511] != 0xAA) {
        uart_puts("ERROR: signature\n");
        return 0;
    }
        
    // parsing first partition entry at 0x1BE
    entry1 = (partition_entry_t*)kmalloc(sizeof(partition_entry_t));
    memcpy(entry1, mbr + 0x1BE, sizeof(partition_entry_t));

    // check partition type should be 0B (FAT32)
    if(entry1->partition_type != 0xB){
        uart_puts("ERROR: Wrong partition type \r\n");
        return 0;
    }
   
    uart_puts("Block index: ");
    uart_hex(entry1->starting_sector);
    uart_puts("\r\n");

    //  boot sector of FAT32 partition_s block
    unsigned char partition_block[BLOCK_SIZE];
    readblock(entry1->starting_sector, partition_block);

    boot_sec = (boot_sector_t*)kmalloc(sizeof(boot_sec));
    memcpy(boot_sec, partition_block, sizeof(boot_sector_t));
    return 1;
}

void set_fat32_vnode(struct vnode* vnode){ 
    vnode->v_ops = fat32_v_ops;
    vnode->f_ops = fat32_f_ops;
}

int setup_mount_fat32(struct filesystem* fs, struct mount* mnt){
    struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
    set_fat32_vnode(vnode);

    struct fat32_node *node = (struct fat32_node*)kmalloc(sizeof(struct fat32_node));
    node->cluster = boot_sec->first_cluster; // setup for root cluster
    vnode->internal = (void*)node;

    struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
    set_dentry(dentry,vnode,"/");
    dentry->flag = ROOT_DIR;
 
    mnt->fs= fs; 
    mnt->root = vnode;
    mnt->dentry = dentry;

    // finding root directory sector index
    root_sec_index = entry1->starting_sector + boot_sec->n_reserved_sectors + (boot_sec->n_sector_per_fat_32 * boot_sec->n_file_alloc_tabs);
    return 0;
}

// Function to find all entry under target directory sector
int load_dent_fat32(struct dentry *dent,char *component_name){
    int flag = -1;
    
    struct fat32_node *fat32_node = (struct fat32_node *)dent->vnode->internal;
    int cluster = fat32_node->cluster;

    unsigned char sector[BLOCK_SIZE];
    fat32_dir_t *dir = (fat32_dir_t *) sector;  
    
    //Read the first block of the cluster
    readblock(root_sec_index + (cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, sector);
    
    uart_puts("Loading file in ");
    uart_puts(dent->dname);
    uart_puts(" directory\r\n");

    //At the end of the directory is a record that begins with zero
    for (int i = 0; dir[i].name[0] != '\0'; i++ ){
        // For 0xE5, it means that the file was deleted
        if(dir[i].name[0] == 0xE5){
            continue;
        }
        
        char name[9];
        strtolower(dir[i].name);
        strcpy_delim(name, dir[i].name, 8, ' ');
    
        char ext[4];
        strtolower(dir[i].ext);
        strncpy(ext, dir[i].ext, 3);
        
        char* complete_name;
        if(dir[i].attr[0] & 0x10){ //it means that this is a subdirectory
            complete_name = name;   
        }else{    
            complete_name = strcat(strcat(name, "."), ext);
        }
        uart_puts("name:");
        uart_puts(complete_name);
        uart_puts(", component_name:");
        uart_puts(component_name);
        uart_puts("\r\n");
        if(strcmp(complete_name, component_name) != 0) continue;

        struct fat32_node *child_fat32_node = (struct fat32_node*)kmalloc(sizeof(struct fat32_node)); 
        strncpy(child_fat32_node->ext, dir[i].ext, 3);
        child_fat32_node->cluster = ((dir[i].cluster_high) << 16) | ( dir[i].cluster_low );
        child_fat32_node->size = dir[i].size;
        
        struct vnode *child_vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
        set_fat32_vnode(child_vnode);
        child_vnode->internal = (void*)child_fat32_node;

        struct dentry* child_dent = (struct dentry*)kmalloc(sizeof(struct dentry));
        set_dentry(child_dent, child_vnode, complete_name);
        child_dent->flag =  dir[i].attr[0] & 0x10 ? DIRECTORY : REGULAR_FILE;     
        child_dent->parent_dentry = dent;

        if(dent->child_count < MAX_CHILD)
            dent->child_dentry[dent->child_count++] = child_dent;
        else{
            uart_puts("child_count > MAX_CHILD \r\n");
            while(1);
        }
        
        // success loading the target dentry
        uart_puts("Name: ");
        uart_puts(child_dent->dname);
        uart_puts(",ext: ");
        uart_puts(child_fat32_node->ext);  
        uart_puts("\r\n");
        flag = 0;   
        break;  
    }
    return flag;
}

int lookup_fat32(struct dentry* dir, struct vnode** target, const char* component_name){
    for(int i = 0; i < dir->child_count; i++){
        if(strcmp(dir->child_dentry[i]->dname, component_name) == 0){
            *target = dir->child_dentry[i]->vnode;
            return 0;
        }
    }   
    return -1; 
}

int write_fat32(struct file* file, const void* buf, size_t len){
    struct vnode* vnode = file->vnode;
    struct fat32_node *node = (struct fat32_node *)vnode->internal;
    char write_sector_buf[BLOCK_SIZE];
    int cluster = node->cluster;

    // get file allocation table
    int fat32[FAT32_ENTRY_PER_BLOCK];
    int total_len = len;

    if((unsigned int)strlen(buf) < len)
        total_len = strlen(buf);

    int write_len = 0;

    while(cluster>1 && cluster < 0xFFF8){ //??
        if(total_len > BLOCK_SIZE){
            strncpy (write_sector_buf, buf, BLOCK_SIZE);
            buf += BLOCK_SIZE;
            total_len -= BLOCK_SIZE;
            write_len += BLOCK_SIZE;

            writeblock(root_sec_index + (cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, write_sector_buf);

            // get the next cluster in chain
            readblock(boot_sec->n_reserved_sectors + entry1->starting_sector + (cluster / FAT32_ENTRY_PER_BLOCK), fat32);
            cluster = fat32[cluster % FAT32_ENTRY_PER_BLOCK] ;
        }else{
            strncpy (write_sector_buf, buf, total_len);
            write_len += total_len;
            writeblock(root_sec_index + (cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, write_sector_buf);
            break;
        }
    }
    node->size = write_len;

    // udpate size in directory entry
    unsigned char sector[BLOCK_SIZE];
    fat32_dir_t *dir = (fat32_dir_t *) sector;
    readblock (root_sec_index + (file->parent_cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, sector);

    for (int i = 0; dir[i].name[0] != '\0'; i++ ){
        // For 0xE5, it means that the file was deleted
        if(dir[i].name[0]==0xE5 || (dir[i].attr[0] & 0x10) ) continue;

        char name[9];
        strtolower(dir[i].name);
        strcpy_delim(name, dir[i].name,8,' ');

        char ext[4];
        strtolower(dir[i].ext);
        strncpy(ext, dir[i].ext, 3);

        char* complete_name;
        complete_name = strcat(strcat(name,"."),ext);

        if (strcmp(file->fname, complete_name) == 0){
            dir[i].size = write_len;
            writeblock (root_sec_index + (file->parent_cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, sector);
            return 0;
        }
    }
    return -1;
}

int read_fat32(struct file* file, void* buf, size_t len){
    struct vnode* vnode = file->vnode;
    struct fat32_node *node = (struct fat32_node *)vnode->internal;
    
    char read_sector_buf[BLOCK_SIZE];
    int cluster = node->cluster;

    // this is the total length we need to read form the start of file
    int total_len;

    // If the required length is more than what we can actually read
    // Then set it to the maximum that we can read
    if(len > (node->size - file->f_pos)){
        total_len = node->size;
    }else{
        total_len = file->f_pos + len;
    }

    // get file allocation table
    int fat32[FAT32_ENTRY_PER_BLOCK];
    char *tmp_buffer = (char*)kmalloc(sizeof(char) * (total_len + 1));
    char *ptr = tmp_buffer;

    int read_len = 0;
    //Last cluster in file (EOC): (0x?FFFFFF8 - 0x?FFFFFFF)
    //Free Cluster: 0x?0000000
    //Reserved for internal purposes: 0x?0000001
    while(cluster > 1 && cluster < 0xFFF8){
        // read data 
        readblock (root_sec_index + (cluster - boot_sec->first_cluster) * boot_sec->logical_sector_per_cluster, read_sector_buf);
        if(total_len > BLOCK_SIZE){
            strncpy (ptr, read_sector_buf, BLOCK_SIZE);
            ptr += BLOCK_SIZE;
            total_len -= BLOCK_SIZE;
            read_len += BLOCK_SIZE;

            // get the next cluster in chain from FAT table
            readblock(entry1->starting_sector + boot_sec->n_reserved_sectors + (cluster / FAT32_ENTRY_PER_BLOCK ), fat32);
            cluster = fat32[cluster % FAT32_ENTRY_PER_BLOCK] ;
        }else{
            strncpy (ptr, read_sector_buf, total_len);
            read_len += total_len;
            break;
        }

    }

    read_len -= file->f_pos;
    strncpy(buf,tmp_buffer + file->f_pos, read_len);

    file->f_pos += read_len;

    kfree((unsigned long)tmp_buffer);
    return read_len;
}
