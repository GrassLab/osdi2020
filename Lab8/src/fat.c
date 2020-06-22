#include "include/vfs.h"
#include "include/uart.h"
#include "include/fs/fat32.h"
#include "include/mm.h"
#include "include/sd.h"

unsigned long get_u32(unsigned char* start){
	unsigned long num=0;
        for(int i=0;i<4;i++){
       		num |= ((unsigned long)start[i] << (8*i));
     	}
        
	return num;
}

int fat_getpartition(){ 
    unsigned char mbr[512];

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
    
    int block_index = entry1->starting_sector;
    printf("### Block index: %d\r\n",block_index);

    int partition_size = entry1->number_of_sector;
    printf("### Partition size: %d\r\n",partition_size);

    //  boot sector of FAT32 partition’s block
    unsigned char partition_block[512];
    readblock(block_index,partition_block);
    
    boot_sec = (boot_sector_t*)kmalloc(sizeof(boot_sec));
    memcpy(boot_sec,partition_block,sizeof(boot_sector_t));
    return 1;
}

void set_fat32fs_vnode(struct vnode* vnode){
         // create root directory's vnode
         vnode->v_ops = fat32fs_v_ops;
         vnode->f_ops = fat32fs_f_ops;
}

int setup_mount_fat32fs(struct filesystem* fs, struct mount* mt){
	struct vnode *vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
        set_fat32fs_vnode(vnode);

	struct dentry *dentry=(struct dentry*)kmalloc(sizeof(struct dentry));
        set_dentry(dentry,vnode,"/");
        dentry->flag = ROOT_DIR;

	mt->fs= fs;
 	mt->root = vnode;
 	mt->dentry = dentry;

        // finding root directory
	unsigned int root_sec = (boot_sec->n_sector_per_fat_32 * boot_sec->n_file_alloc_tabs ) + boot_sec->n_reserved_sectors;

	unsigned char sector[512];
        fat32_dir_t *dir = (fat32_dir_t *) sector;	

        readblock (entry1->starting_sector + root_sec, sector);

	printf("### Loading file in root directory\r\n");
	for (int i = 0; dir[i].name[0] != '\0'; i++ ){
		// For 0xE5, it means that the file was deleted
		if(dir[i].name[0]==0xE5 ) continue;

		struct vnode *child_vnode = (struct vnode*)kmalloc(sizeof(struct vnode));
 		set_fat32fs_vnode(child_vnode);

		struct fat32fs_node *child_fat32fs_node = (struct fat32fs_node*)kmalloc(sizeof(struct fat32fs_node)); 
		strncpy(child_fat32fs_node->ext,dir[i].ext,3);
		child_fat32fs_node->cluster = ((dir[i].cluster_high) << 16) | ( dir[i].cluster_low );
		child_fat32fs_node->size = dir[i].size;
		child_vnode->internal = (void*)child_fat32fs_node;

		struct dentry* child_dent = (struct dentry*)kmalloc(sizeof(struct dentry));
		char name[9];
		strncpy(name, dir[i].name,8);
		set_dentry(child_dent,child_vnode,name);
		child_dent->parent_dentry = mt->dentry;
		child_dent->flag = dir[i].attr[0] & 0x10 ? DIRECTORY : REGULAR_FILE;
		

	 	if(mt->dentry->child_count < MAX_CHILD)
                  	mt->dentry->child_dentry[mt->dentry->child_count++] = child_dent;
          	else{
                  	printf("NOT HANDLE THIS RIGHt NOW!\r\n");
                  	while(1);
         	}
		
		printf("name: %s, ext: %s\r\n",child_dent->dname,child_fat32fs_node->ext);
	}
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

/*
int create_fat32fs(struct dentry* dir, struct vnode** target, \
                  const char* component_name){

}

int mkdir_fat32fs(struct dentry* dir, struct vnode** target, const char *component_name){

}

int write_fat32fs(struct file* file, const void* buf, size_t len){

}

int read_fat32fs(struct file* file, void* buf, size_t len){
}*/
