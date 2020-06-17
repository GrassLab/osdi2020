#include "uart.h"
#include "mystd.h"
#include "sdhost.h"
#include "fat32.h"
#include "vfs.h"

struct partition_entry sd_root_partition_entry;
struct filesystem fs;
struct file_descriptor_operations FAT32_FD_OPS;

int fat32_read(struct file_descriptor *fd, char *buf, int len){
	struct vnode *file_vnode = fd->vnode;

	// read data to vnode
	char read_sector[512];
	readblock((file_vnode->cluster-2) * sd_root_partition_entry.logic_sector_per_cluster + sd_root_partition_entry.cluster_start, read_sector );
	for(int j=0; j<512; j++){
		file_vnode->mem[j] = read_sector[j];
	}
	file_vnode->mem[511] = '\0';

	// write back to user
	int fd_start = fd->f_pos;
    int size = 0;

    for(size=0; size<len; size++){
        if(file_vnode->mem[fd_start + size] == '\0') break;
        buf[size] = file_vnode->mem[fd_start + size];
    }

    return size;
}

int fat32_write(struct file_descriptor* fd, char* buf, int len){
    int fd_start = fd->f_pos;
    struct vnode *file_vnode = fd->vnode;

    // write to buffer
    for(int i=0; i<len; i++){
        file_vnode->mem[fd_start + i] = buf[i];
    }

    // write buffer back to fat32
    int size = strlen(file_vnode->mem);
    writeblock((file_vnode->cluster-2) * sd_root_partition_entry.logic_sector_per_cluster + sd_root_partition_entry.cluster_start, file_vnode->mem );

    // modify FAT directory table
    char sector[512];
    struct fat32_directory *fat32_dirs = (struct fat32_directory*)sector;
    readblock(sd_root_partition_entry.cluster_start, sector );
    fat32_dirs[file_vnode->fat_dir_id].size = size;
    writeblock(sd_root_partition_entry.cluster_start, sector );

    return 1;
}

int fat32_setup_mount(struct filesystem *fs, struct mount *mount){
	uart_puts("fat32_setup_mount\n");
	
	mount->fs = fs;
	mount->root = fs->root;

	// get fat32 directory 
	char sector[SECTOR_SIZE];
	struct fat32_directory *fat32_dirs = (struct fat32_directory*)sector;

	// move to FAT, than move over reserved sectors and FAT tables
	readblock(sd_root_partition_entry.cluster_start, sector );

	uart_puts("fat32 mounted, read cluster\n");

	for(int i=0; fat32_dirs[i].name[0]!='\0'; i++){
		if(fat32_dirs[i].name[0]<'A' || fat32_dirs[i].name[0]>'Z') continue;

		// new dentry
		struct dentry *new_dentry = &dentryObj[dentryObjNum];
    	dentryObjNum++;

    	// file name
    	int id=0;
    	for(int j=0; j<8 && fat32_dirs[i].name[j]!=' '; j++, id++) new_dentry->name[id] = fat32_dirs[i].name[j];
    	new_dentry->name[id] = '.'; id++;    	
    	for(int j=0; j<3 && fat32_dirs[i].ext[j]!=' '; j++, id++) new_dentry->name[id] = fat32_dirs[i].ext[j];
    	new_dentry->name[id] = '\0';

    	uart_puts("[FILE NAME]: ");
    	uart_puts(new_dentry->name);
    	uart_puts("\n");

    	// file type directory or file
    	if(fat32_dirs[i].attr[0] & isSUBDIRECTORY) {
    		new_dentry->type = directory;
    	}
    	else{
    		new_dentry->type = file;

    		// new vnode
    		struct vnode *new_vnode = &vnodeObj[vnodeObjNum];
	        vnodeObjNum++;

	        // FAT 32 directory id
	        new_vnode->fat_dir_id = i;
	        // init memory
	        for(int i=0; i<1024; i++) new_vnode->mem[i] = '\0';
	        // get cluster position and size
	        new_vnode->cluster = ( ( ( uint32_t ) ( fat32_dirs[i].cluster_high ) ) << 16 ) | ( fat32_dirs[i].cluster_low );
	    	new_vnode->size = fat32_dirs[i].size;
	    	// add new vnode to dentry
	        new_dentry->vnode = new_vnode;

	        char read_sector[512];
	    	// get file info
	    	readblock((new_vnode->cluster-2) * sd_root_partition_entry.logic_sector_per_cluster + sd_root_partition_entry.cluster_start, read_sector );
	    	for(int j=0; j<512; j++){
	    		new_vnode->mem[j] = read_sector[j];
	    	}
	    	new_vnode->mem[511] = '\0';
	    	// operation
	    	new_vnode->fd_ops = &FAT32_FD_OPS;

	    	uart_puts("[CONTENT]: ");
	    	uart_puts(new_vnode->mem);
	    	uart_puts("\n[CONTENT END]\n");
    	} 

    	// dir info
    	new_dentry->parent = fs->root;
    	fs->root->d_list[ fs->root->count ].used = 1;
    	fs->root->d_list[ fs->root->count ].child = new_dentry;
    	fs->root->count++;
	}

	return 1;
}

void init_fat32(){

	uart_puts("init_fat32\n");
	sd_init();

	// operation init
	FAT32_FD_OPS.write = fat32_write;
	FAT32_FD_OPS.read = fat32_read;

	// get root partition entry
	char sector[SECTOR_SIZE];

	readblock(0, sector);
	for(int i=0; i<16; i++) {
		((char*)(&sd_root_partition_entry))[i] = sector[FIRST_PARTITION_ENTRY + i];
	}

	// get root first boot sector
	readblock(sd_root_partition_entry.start_sector, sector);
	struct boot_sector *sd_root_boot_sector = (struct boot_sector *)&sector;

	uart_puts("read root boot sector\n");

	sd_root_boot_sector->bytes_per_logic_sector = CONCATE_8 ( sector[11], sector[12] );
    sd_root_boot_sector->n_root_dir_entries     = CONCATE_8 ( sector[17], sector[18] );

    // FAT structure: reserved + FAT table
    sd_root_partition_entry.root_sector = sd_root_boot_sector->n_reserved_sectors + (sd_root_boot_sector->n_sector_per_fat_32*sd_root_boot_sector->n_file_alloc_tabs);

    // MBR -> FAT (reserved + FAT) -> cluster
    sd_root_partition_entry.cluster_start = sd_root_partition_entry.start_sector + sd_root_partition_entry.root_sector;
    sd_root_partition_entry.logic_sector_per_cluster = sd_root_boot_sector->logic_sector_per_cluster;

    fs.name = "fat32";
    fs.setup_mount = fat32_setup_mount;
    uart_puts("setup fs\n");

    register_filesystem(&fs);
}
