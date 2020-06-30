#include "include/vfs.h"
#include "include/uart.h"
#include "include/fs/fat32.h"
#include "include/mm.h"
#include "include/sd.h"

// global for recording required block index 
unsigned int root_sec_index = 0;

int fat_getpartition(){ 
    unsigned char mbr[BLOCK_SIZE];
    // read the MBR, find the boot sector
    readblock(0,mbr); 
        
    // check magic
    if(mbr[0x1FE]!=0x55 || mbr[0x1FF]!=0xAA) {
    	printf("ERROR: Bad magic in MBR\n");
    	return 0;
    }
   
    // parsing first partition entry
    entry1 = (partition_entry_t*)kmalloc(sizeof(partition_entry_t));
    memcpy(entry1,mbr+0x1BE,sizeof(partition_entry_t));

    // check partition type
    if(entry1->partition_type!=0xB){
    	printf("ERROR: Wrong partition type %d\r\n",entry1->partition_type);
        return 0;
    }
    
    printf("### FAT32 with CHS addressing\r\n");
    printf("### Block index: %d\r\n",entry1->starting_sector);
    printf("### Partition size: %d\r\n",entry1->number_of_sector);

    //  boot sector of FAT32 partition’s block
    unsigned char partition_block[BLOCK_SIZE];
    readblock(entry1->starting_sector,partition_block);
    
    boot_sec = (boot_sector_t*)kmalloc(sizeof(boot_sec));
    memcpy(boot_sec,partition_block,sizeof(boot_sector_t));

    return 1;
}

void set_fat32fs_vnode(struct vnode* vnode){
         // create root directory's vnode
         vnode->v_ops = fat32fs_v_ops;
         vnode->f_ops = fat32fs_f_ops;
}

// Function to find all entry under target directory sector
int load_dent_fat32(struct dentry *dent,char *component_name){
	int flag = -1;
	
	struct fat32fs_node *fat32fs_node = (struct fat32fs_node *)dent->vnode->internal;
	int cluster = fat32fs_node->cluster;

	unsigned char sector[BLOCK_SIZE];
        fat32_dir_t *dir = (fat32_dir_t *) sector;	

	readblock (root_sec_index + \
                     (cluster-boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
                       sector);

	printf("### Loading file in %s directory\r\n", dent->dname);
	for (int i = 0; dir[i].name[0] != '\0'; i++ ){
		// For 0xE5, it means that the file was deleted
		if(dir[i].name[0]==0xE5 ) continue;
		char name[9];
		strtolower(dir[i].name);
		strcpy_delim(name, dir[i].name,8,' ');
	
		char ext[4];
		strtolower(dir[i].ext);
		strncpy(ext, dir[i].ext, 3);
		
		char* complete_name;
		if(dir[i].attr[0] & 0x10)
			complete_name = name;	
		else	
			complete_name = strcat(strcat(name,"."),ext);

		if(strcmp(complete_name,component_name) != 0) continue;
		
		struct vnode *child_vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
 		set_fat32fs_vnode(child_vnode);

		struct fat32fs_node *child_fat32fs_node = (struct fat32fs_node*)kmalloc(sizeof(struct fat32fs_node)); 
		strncpy(child_fat32fs_node->ext, dir[i].ext, 3);
		child_fat32fs_node->cluster = ((dir[i].cluster_high) << 16) | ( dir[i].cluster_low );
		child_fat32fs_node->size = dir[i].size;
		child_fat32fs_node->parent_cluster = cluster;
		strcpy(child_fat32fs_node->name,complete_name);
		child_vnode->internal = (void*)child_fat32fs_node;

		struct dentry* child_dent = (struct dentry*)kmalloc(sizeof(struct dentry));
		set_dentry(child_dent,child_vnode,complete_name);
		child_dent -> flag =  dir[i].attr[0] & 0x10 ? DIRECTORY : REGULAR_FILE;		
		child_dent->parent_dentry = dent;

	 	if(dent->child_count < MAX_CHILD)
                  	dent->child_dentry[dent->child_count++] = child_dent;
          	else{
                  	printf("NOT HANDLE THIS RIGHt NOW!\r\n");
                  	while(1);
         	}
		
		// success loading the target dentry
		printf("Name: %s, ext: %s\r\n",child_dent->dname,child_fat32fs_node->ext);	
		flag = 0;	
		break;	
	}
	return flag;
}

int setup_mount_fat32fs(struct filesystem* fs, struct mount* mt){
	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
        set_fat32fs_vnode(vnode);

	struct fat32fs_node *fat32fs_node = (struct fat32fs_node*)kmalloc(sizeof(struct fat32fs_node));
	fat32fs_node->cluster = boot_sec->first_cluster; // setup for root cluster
	vnode->internal = (void*)fat32fs_node;

	struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
        set_dentry(dentry,vnode,"/");
        dentry->flag = ROOT_DIR;

	mt->fs= fs;
 	mt->root = vnode;
 	mt->dentry = dentry;

        // finding root directory
	root_sec_index = entry1->starting_sector + (boot_sec->n_sector_per_fat_32 * boot_sec->n_file_alloc_tabs ) + boot_sec->n_reserved_sectors;

	return 0;
}
/*
void ls_fat32fs(struct dentry* dir){

}
*/
int lookup_fat32fs(struct dentry* dir, struct vnode** target, \
                 const char* component_name){
	
	for(int i=0;i<dir->child_count;i++){
		if(strcmp(dir->child_dentry[i]->dname, component_name)==0){
                        *target = dir->child_dentry[i]->vnode;
                        return 0;
                }
        }
	return -1;
}

/** Bad design, but anyway **/
int cpy(char *dest, const char *src)
{
         int c = 0;
         while (src[c] != '\0'){
         	dest[c] = src[c];
                c++;
         }
  
    	return c;
}

/****/
int create_fat32fs(struct dentry* dent, struct vnode** target, \
                  const char* component_name){

	struct fat32fs_node *fat32fs_node = (struct fat32fs_node *)dent->vnode->internal;
 	int cluster = fat32fs_node->cluster;

	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
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
	for(i = 0;component_name[i] != '\0';i++){
		if(component_name[i]=='.'){
			dot = i;
			continue;
		}
		
		if(dot==0)
			name[i] = component_name[i];
		else
			ext[i-dot-1] = component_name[i];
		
	}
	
	name[dot] = '\0';
	ext[i-dot-1] = '\0';

	// Find an empty entry in the FAT table
	int fat32[FAT32_ENTRY_PER_BLOCK];
	int empty_cluster = 0;
	int find = 0;

	while(1){ // we assume that empty cluster will exist
		readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                  (empty_cluster / FAT32_ENTRY_PER_BLOCK ),fat32);

		for(i=0;i<FAT32_ENTRY_PER_BLOCK;i++){
			if(fat32[i] == 0){
				// Now, created file can use only one cluster
				fat32[i] = 0xFFFF;
				find = 1;
				break;
			}
		}

 		if(find==1){
			writeblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                  (empty_cluster / FAT32_ENTRY_PER_BLOCK ),fat32);
			
			empty_cluster += i;
			break;
		}
		else
			empty_cluster += FAT32_ENTRY_PER_BLOCK;		
	}
	// Find an empty directory entry in the target directory
	
	// Note: since the sd driver will read file name as large letter, 
	// so we need to deal with it...
	int ent_idx = 0;
	for (; dir[ent_idx].name[0] != 0; ent_idx++ ){
		 if(dir[ent_idx].name[0]==0xE5) break;
                 int c = ((dir[ent_idx].cluster_high) << 16) | ( dir[ent_idx].cluster_low);
		 readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                   (c / FAT32_ENTRY_PER_BLOCK ),fat32);
	
		 strtolower(dir[ent_idx].name);
		 strtolower(dir[ent_idx].ext);	
		 
	}
	
	printf("Create new file at cluster: %d ,entry %d \r\n",empty_cluster,ent_idx);
	
	// set an fat32 directory entry(we just set the needed infomation)
	memzero((unsigned long)(dir + ent_idx), sizeof(fat32_dir_t));
	cpy(dir[ent_idx].name,name);
	cpy(dir[ent_idx].ext,ext);
	dir[ent_idx].attr[0] = 0x20;
	dir[ent_idx].cluster_high = empty_cluster >> 16;
	dir[ent_idx].cluster_low = (empty_cluster<<16) >> 16;
	// write back to create new file
	writeblock (root_sec_index + \
                        (cluster-boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
                         sector);

	struct fat32fs_node* child_node = (struct fat32fs_node*)kmalloc(sizeof(struct fat32fs_node));
 	strcpy(child_node->name,component_name);
	strcpy(child_node->ext,ext);
	child_node->size = 0;
 	child_node->parent_cluster = cluster;
	child_node->cluster = empty_cluster;	
	vnode->internal = (void *)fat32fs_node;
 
 	struct dentry* child = (struct dentry*)kmalloc(sizeof(struct dentry));
 	set_dentry(child,vnode,component_name);
 	child->parent_dentry = dent;
 	child->flag = REGULAR_FILE;
 
        if(dent->child_count<MAX_CHILD)
        	dent->child_dentry[dent->child_count++] = child;
        else{
     		printf("NOT HANDLE THIS RIGHt NOW!\r\n");
      		while(1);
        }
 
        printf("### Create file %s\r\n",dent->child_dentry[dent->child_count-1]->dname);
 
        *target = vnode;
	
	return -1;
}
/*
int mkdir_fat32fs(struct dentry* dir, struct vnode** target, const char *component_name){

}
*/
int write_fat32fs(struct file* file, const void* buf, size_t len){
	struct fat32fs_node *node = (struct fat32fs_node *)file->vnode->internal;
	char write_sector_buf[BLOCK_SIZE];
	int cluster = node->cluster;
	
	// get file allocation table
	int fat32[FAT32_ENTRY_PER_BLOCK];
	
	int total_len = len;

	if((unsigned int)strlen(buf) < len)
		total_len = strlen(buf);
	

	int write_len = 0;

	while(cluster>1 && cluster < 0xFFF8){
		if(total_len > BLOCK_SIZE){
			strncpy (write_sector_buf, buf, BLOCK_SIZE);
			buf += BLOCK_SIZE;
			total_len -= BLOCK_SIZE;
			write_len += BLOCK_SIZE;
		
			writeblock(root_sec_index + \
				(cluster-boot_sec->first_cluster )*boot_sec->logical_sector_per_cluster,\
	       			write_sector_buf);
		
		
			// get the next cluster in chain
                        readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
                                 (cluster / FAT32_ENTRY_PER_BLOCK ),fat32);
                        
			cluster = fat32[cluster % FAT32_ENTRY_PER_BLOCK] ;
		}
		else{
			strncpy (write_sector_buf, buf, total_len);
			write_len+=total_len;

			writeblock(root_sec_index + \
				(cluster-boot_sec->first_cluster )*boot_sec->logical_sector_per_cluster,\
	       			write_sector_buf);
			break;
		}
	}		
	node->size = write_len;

	// udpate size in directory entry
	unsigned char sector[BLOCK_SIZE];
        fat32_dir_t *dir = (fat32_dir_t *) sector;
    	readblock (root_sec_index + \
		 (node->parent_cluster - boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
		 sector);

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

       		if (strcmp(node->name, complete_name)==0){
            		dir[i].size = write_len;
			writeblock (root_sec_index + \
			 	(node->parent_cluster - boot_sec->first_cluster)*boot_sec->logical_sector_per_cluster,\
				sector);

            		return 0;
        	}
	}

	return -1;	
}

int read_fat32fs(struct file* file, void* buf, size_t len){
	struct fat32fs_node *node = (struct fat32fs_node *)file->vnode->internal;
	char read_sector_buf[BLOCK_SIZE];
	int cluster = node->cluster;

	// this is the total length we need to read form the start of file
	int total_len;
	
	// If the required length is more than what we can actually read
	// Then set it to the maximum that we can read
	if ( len > ( node->size - file->f_pos ) ){
		total_len = node->size;
	}
	else{
		total_len = file->f_pos + len;
	}
	
	// get file allocation table 
	int fat32[FAT32_ENTRY_PER_BLOCK];	
	char *tmp_buffer = (char*)kmalloc(sizeof(char) * (total_len+1));
	char *ptr = tmp_buffer;

	int read_len = 0;
	while(cluster>1 && cluster < 0xFFF8){	
		readblock (root_sec_index + \
			( cluster - boot_sec->first_cluster ) * boot_sec->logical_sector_per_cluster,\
	       		read_sector_buf);
		
		if( total_len > BLOCK_SIZE){
			strncpy (ptr, read_sector_buf, BLOCK_SIZE);
			ptr += BLOCK_SIZE; 
			total_len -= BLOCK_SIZE;
			read_len += BLOCK_SIZE;

			// get the next cluster in chain
			readblock(boot_sec->n_reserved_sectors + entry1->starting_sector +\
				(cluster / FAT32_ENTRY_PER_BLOCK ),fat32);
			cluster = fat32[cluster % FAT32_ENTRY_PER_BLOCK] ;
		}
		else{		
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
