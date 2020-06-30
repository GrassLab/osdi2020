#include "vfs.h"
#include "uart.h"
#include "fat32.h"
#include "mm.h"
#include "sd.h"

// global for recording required block index 
unsigned int root_sec_index = 0;

int fat_getpartition(){ 
    unsigned char mbr[BLOCK_SIZE];
    // read the MBR, find the boot sector
    readblock(0,mbr); 
        
    // check magic
    if(mbr[0x1FE]!=0x55 || mbr[0x1FF]!=0xAA) {
    	printf("ERROR: has something wrong in MBR\n");
    	return 0;
    }
   
    // parsing first partition entry
    entry1 = (partition_entry_t*)kmalloc(sizeof(partition_entry_t));
    // memcpy -> from, dst, sizeof
    _memcpy(mbr+0x1BE, entry1, sizeof(partition_entry_t));

    // check partition type
    if(entry1->partition_type!=0xB){
    	printf("ERROR: Wrong partition type %d\r\n", entry1->partition_type);
        return 0;
    }
    
    printf("### FAT32 with CHS addressing\r\n");
    printf("### Block index: %d\r\n", entry1->starting_sector);
    printf("### Partition size: %d\r\n", entry1->number_of_sector);

    //  boot sector of FAT32 partition’s block
    unsigned char partition_block[BLOCK_SIZE];
    readblock(entry1->starting_sector, partition_block);
    
    boot_sector_ = (boot_sector_t*)kmalloc(sizeof(boot_sector_t));
    _memcpy(partition_block, boot_sector_, sizeof(boot_sector_t));

    return 1;
}

void set_fat32fs_vnode(vnode_t* vnode){
    // create root directory's vnode
    vnode->v_ops = fat32fs_v_ops;
    vnode->f_ops = fat32fs_f_ops;
}


int setup_mount_fat32fs(filesystem_t* fs, mount_t* mt){
	vnode_t *vnode = (vnode_t*)kmalloc(sizeof(vnode_t));
    set_fat32fs_vnode(vnode);

	fat32fs_node_t *fat32fs_node = (fat32fs_node_t*)kmalloc(sizeof(fat32fs_node_t));
    // setup for root cluster
	fat32fs_node->cluster = boot_sector_->first_cluster; 
	vnode->internal = (void*)fat32fs_node;

	dentry_t *dentry = (dentry_t*)kmalloc(sizeof(dentry_t));
    set_dentry(dentry, vnode, "/");
    dentry->flag = ROOT_DIR;

	mt->fs = fs;
 	mt->root = vnode;
 	mt->dentry = dentry;

    // finding root directory
	root_sec_index = entry1->starting_sector + (boot_sector_->n_sector_per_fat_32 * boot_sector_->n_file_alloc_tabs) + boot_sector_->n_reserved_sectors;

	return 0;
}

int lookup_fat32fs(struct dentry* dir, struct vnode** target, \
                 const char* component_name){
	for(int i = 0; i < dir->child_count; i++){
		if(strcmp(dir->child_dentry[i]->dname, component_name) == 0){
                *target = dir->child_dentry[i]->vnode;
                return 0;
            }
        }
	return -1;
}

int create_fat32fs(dentry_t* dent, vnode_t** target, const char* component_name){

	fat32fs_node_t *fat32fs_node = (fat32fs_node_t *)dent->vnode->internal;
 	int cluster = fat32fs_node->cluster;

	vnode_t *vnode = (vnode_t*)kmalloc(sizeof(vnode_t));
    set_fat32fs_vnode(vnode);

	unsigned char sector[BLOCK_SIZE];
	fat32_dir_t *dir =  (fat32_dir_t *)sector;
	readblock (root_sec_index + \
              (cluster-boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
              sector);
	
	// parsing for EXT
	char name[9];
	char ext[4];
	unsigned int i;
	unsigned int dot = 0;
	for(i = 0; component_name[i] != '\0'; i++){
		if(component_name[i] == '.'){
			dot = i;
			continue;
		}
		
		if(dot == 0){
			name[i] = component_name[i];
        }
		else{
			ext[i-dot-1] = component_name[i];
        }
	}
	
	name[dot] = '\0';
	ext[i-dot-1] = '\0';

	// Find an empty entry in the FAT table
	int fat32[FAT32_ENTRY_PER_BLOCK];
	int empty_cluster = 0;
	int find = 0;

    //Find an empty entry in the FAT table, we assume that empty cluster will exist
	while(1){ 
		readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                  (empty_cluster / FAT32_ENTRY_PER_BLOCK ),fat32);

		for(i = 0; i < FAT32_ENTRY_PER_BLOCK; i++){
			if(fat32[i] == 0){
				// Now, created file can use only one cluster
				fat32[i] = 0xFFFF;
				find = 1;
				break;
			}
		}

 		if(find == 1){
			writeblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                  (empty_cluster / FAT32_ENTRY_PER_BLOCK ),fat32);
			
			empty_cluster += i;
			break;
		}
		else{
			empty_cluster += FAT32_ENTRY_PER_BLOCK;		
        }
	}
	// Find an empty directory entry in the target directory
	
	// Note: since the sd driver will read file name as large letter, 
	// so we need to deal with it...
	int ent_idx = 0;
	for (; dir[ent_idx].name[0] != 0; ent_idx++){
        if(dir[ent_idx].name[0] == 0xE5){
            break;
        } 
        int c = ((dir[ent_idx].cluster_high) << 16) | (dir[ent_idx].cluster_low);
        readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                        (c / FAT32_ENTRY_PER_BLOCK ),fat32);
	
        // strtolower(dir[ent_idx].name);
        // strtolower(dir[ent_idx].ext);	
	}
	
	printf("Create new file at cluster: %d ,entry %d \r\n",empty_cluster,ent_idx);
	
	// set an fat32 directory entry(we just set the needed infomation)
	memzero((unsigned long)(dir + ent_idx), sizeof(fat32_dir_t));
	cpy(dir[ent_idx].name, name);
	cpy(dir[ent_idx].ext, ext);
	dir[ent_idx].attr[0] = 0x20;
	dir[ent_idx].cluster_high = empty_cluster >> 16;
	dir[ent_idx].cluster_low = (empty_cluster << 16) >> 16;
	// write back to create new file
	writeblock(root_sec_index + \
              (cluster-boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
              sector);

	fat32fs_node_t* child_node = (sfat32fs_node_t*)kmalloc(sizeof(fat32fs_node_t));
 	strcpy(child_node->name, component_name);
	strcpy(child_node->ext, ext);
	child_node->size = 0;
 	child_node->parent_cluster = cluster;
	child_node->cluster = empty_cluster;	
	vnode->internal = (void *)fat32fs_node;
 
 	dentry_t* child = (dentry_t*)kmalloc(sizeof(dentry_t));
 	set_dentry(child, vnode, component_name);
 	child->parent_dentry = dent;
 	child->flag = REGULAR_FILE;
 
    if(dent->child_count < MAX_CHILD){
        dent->child_dentry[dent->child_count++] = child;
    }
    else{
        printf("//TO DO!\r\n");
        while(1);
    }

    printf("### Create file %s\r\n",dent->child_dentry[dent->child_count-1]->dname);

    *target = vnode;
	
	return -1;
}

// Function to find all dentry under target directory sector
int load_dent_fat32(dentry_t *dent, char *component_name){
	int flag = -1;
	
	fat32fs_node_t *fat32fs_node = (fat32fs_node_t *)dent->vnode->internal;
	int cluster = fat32fs_node->cluster;

	unsigned char sector[BLOCK_SIZE];
    fat32_dir_t *dir = (fat32_dir_t *)sector;	

	readblock(root_sec_index + (cluster - boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster, sector);
	printf("### Loading file in %s directory\r\n", dent->dname);
	for (int i = 0; dir[i].name[0] != '\0'; i++){
		// For 0xE5, it means that the file was deleted
		if(dir[i].name[0] == 0xE5){
            continue;
        } 
        //combine file name and file type to get complete file name
		char name[9];
        _memset(name, '\0', 9);
		// strtolower(dir[i].name);
		strcpy_delim(name, dir[i].name, 8, ' ');
	
		char file_type[4];
        _memset(file_type, '\0', 4);
		// strtolower(dir[i].file_type);
		strncpy(file_type, dir[i].file_type, 3);
		
		char* complete_name;
		if(dir[i].attr[0] && 0x10){
			complete_name = name;	
        }
		else{
			complete_name = strcat(strcat(name,"."), file_type);
        }	

        //if name is not the same, find continue
		if(strcmp(complete_name, component_name) != 0){
            continue;
        } 

		vnode_t *child_vnode = (vnode_t*)kmalloc(sizeof(vnode_t));
 		set_fat32fs_vnode(child_vnode);

		fat32fs_node_t *child_fat32fs_node = (fat32fs_node_t*)kmalloc(sizeof(fat32fs_node_t)); 
		strncpy(child_fat32fs_node->ext, dir[i].ext, 3);
		child_fat32fs_node->cluster = ((dir[i].cluster_high) << 16) | (dir[i].cluster_low);
		child_fat32fs_node->size = dir[i].size;
		child_fat32fs_node->parent_cluster = cluster;
		strcpy(child_fat32fs_node->name, complete_name);
		child_vnode->internal = (void*)child_fat32fs_node;

		dentry_t* child_dent = (dentry_t*)kmalloc(sizeof(dentry_t));
		set_dentry(child_dent, child_vnode, complete_name);
		child_dent->flag = dir[i].attr[0] & 0x10 ? DIRECTORY : REGULAR_FILE;		
		child_dent->parent_dentry = dent;

	 	if(dent->child_count < MAX_CHILD){
            dent->child_dentry[dent->child_count++] = child_dent;
        }
        else{
            printf("//TO DO\n");
            while(1);
        }
		
		// success loading the target dentry
		printf("Name: %s, ext: %s\r\n", child_dent->dname, child_fat32fs_node->ext);	
		flag = 0;	
		break;	
	}
	return flag;
}