#include "sd.h"
#include "vfs.h"
#include "fat32.h"
#include "mm.h"

static int write(file_t *file, const void *buf, size_t len);
static int read(file_t *file, void *buf, size_t len);
static int create(vnode_t *dir_node, vnode_t **target, const char *component_name);
static int lookup(vnode_t *dir_node, vnode_t **target, const char *component_name);
static fat32_node_t *internal_node_create(fat32_node_t *parent,
										unsigned int cluster_index,
										unsigned int dir_entry,
										unsigned int dir_index);

static vnode_operations_t _v_ops = {.create = create,.lookup = lookup };
static file_operations_t _f_ops = {.read = read,.write = write };

static vnode_operations_t *v_ops = &_v_ops;
static file_operations_t *f_ops = &_f_ops;
static unsigned int calculated_offset(fat32_node_t *node);

static unsigned int fat32_cluster_value(fat32_info_t *info, unsigned int index)
{
	unsigned int offset;
	//total chain_length = 512/4
	unsigned int fat[CHAIN_LENGTH];

    // find FAT
	offset = info->logical_block_address + info->count_of_reserved + (index / CHAIN_LENGTH);
	readblock(offset, fat);
	return fat[index % CHAIN_LENGTH];
}

//using fat32 node to get dir entry
static int fat32_node_dir_entry(fat32_node_t *node, directory_entry_t *dir_entry)
{
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	if(node->dir_index >= DIR_LEN)
		return 1;
	if(node->cluster_index == node->info.root_clstr_index)
	{
		dir_entry->size = 0;
		return 0;
	}
	offset = calculated_offset(node);
	readblock(offset + node->dir_entry - node->info.root_clstr_index, dirs);
	*dir_entry = dirs[node->dir_index];
	return 0;
}

static int fat32_node_dir_entry_set(fat32_node_t *node, directory_entry_t *dir_entry)
{
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	if(node->dir_index >= DIR_LEN)
		return 1;
	if(node->cluster_index == node->info.root_clstr_index)
	{
		dir_entry->size = 0;
		return 0;
	}
	offset = calculated_offset(node);
	readblock(offset + node->dir_entry - node->info.root_clstr_index, dirs);
	dirs[node->dir_index] = *dir_entry;
	writeblock(offset + node->dir_entry - node->info.root_clstr_index,
			dirs);
	return 0;
}

static int write(file_t *file, const void *buf, size_t len)
{
    fat32_node_t *node = file->vnode->internal;
	directory_entry_t dir;
	size_t valid_len, pos, start_pos, end_pos;
	unsigned int value, offset;
	unsigned int data_size;
	char data[BLOCK_SIZE];
	// type error
	if(fat32_node_dir_entry(node, &dir))
		return -1;

	printf("this file size is %d\n",dir.size);
	if(dir.size == 0)
		return -1;
	// TODO: find free cluster for new data
	// block full
	data_size = dir.size / BLOCK_SIZE;
	if(dir.size % BLOCK_SIZE)
		data_size++;
	data_size *= BLOCK_SIZE;
	if(file->f_pos >= data_size)
		return 0;
	valid_len = data_size - file->f_pos;
	if(len < valid_len)
		valid_len = len;
	// traversal all block to find target
	// find block offset of fat cluster chain
	value = node->cluster_index;
	// find offset of data region
	offset = calculated_offset(node);
	pos = 0;
	while((value & CHAIN_EOF) != CHAIN_EOF)
	{
		if(pos >= file->f_pos + valid_len)
			break;
		if(pos + BLOCK_SIZE > file->f_pos)
		{
			start_pos =(pos < file->f_pos) ? file->f_pos : pos;
			end_pos = pos + BLOCK_SIZE;
			if(end_pos > file->f_pos + valid_len)
				end_pos = file->f_pos + valid_len;
			// read block
			readblock(offset + value - node->info.root_clstr_index, data);
			// modify block
			_memcpy(buf +(start_pos - file->f_pos), 
					data +(start_pos % BLOCK_SIZE),
					end_pos - start_pos);
			// write back
			writeblock(offset + value - node->info.root_clstr_index, data);
		}
		value = fat32_cluster_value(&node->info, value);
		pos += BLOCK_SIZE;
	}
	file->f_pos += valid_len;
	if(dir.size < file->f_pos)
	{
		dir.size = file->f_pos;
		fat32_node_dir_entry_set(node, &dir);
	}
	return valid_len;
}

static int read(file_t *file, void *buf, size_t len)
{
	fat32_node_t *node;
	directory_entry_t dir;
	size_t pos, start_pos, end_pos;
	unsigned int value, offset;
	char data[BLOCK_SIZE];
	node = file->vnode->internal;
	if(fat32_node_dir_entry(node, &dir))
		return -1;
	printf("this file size is %d\n",dir.size);
	value = node->cluster_index;
	offset = calculated_offset(node);
	pos = file->f_pos;
	readblock(offset + value - node->info.root_clstr_index, data);
	for(int i = 0; i < 100; i++){
		printf("%c", data[i]);
	}
	printf("\n");
	_memcpy(data +(file->f_pos % BLOCK_SIZE), buf, end_pos - file->f_pos);
	file->f_pos += len;
	return len;
}

static int filename_cmp(directory_entry_t *dir, const char *filename)
{
	int name_ind, ext_ind;
	const char *extname;
	for(name_ind = 0; name_ind < 8; ++name_ind)
	{
		if(filename[name_ind] == '\0')
			return dir->name[name_ind] != '\x20';
		if(filename[name_ind] == '.')
		{
			if(dir->name[name_ind] != '\x20')
				return 1;
			break;
		}
		if(filename[name_ind] != dir->name[name_ind])
			return 1;
	}
	if(filename[name_ind] == '\0')
		return 0;
	extname = &filename[name_ind + 1];
	for(ext_ind = 0; ext_ind < 3; ++ext_ind)
	{
		if(extname[ext_ind] == '\0')
			return dir->extension[ext_ind] != '\x20';
		if(extname[ext_ind] != dir->extension[ext_ind])
			return 1;
	}
	return 0;

	if(dir->name[name_ind] != '\x20')
		return 1;
	if(filename[name_ind] == '\0' && dir->name[name_ind] == '\x20'&& dir->extension[0] == '\x20')
		return 0;
	++name_ind;
	for(ext_ind = 0; ext_ind < 3; ++ext_ind)
	{
		if(filename[name_ind + ext_ind] == '\0')
			break;
	}
}

static unsigned int calculated_offset(fat32_node_t *node){
	return node->info.logical_block_address + 
			node->info.count_of_reserved + 
			node->info.fat_num * node->info.sectors_num_per_fat;
} 

static int lookup(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
	vnode_t *new_vnode;
	fat32_node_t *node;
	directory_entry_t dir;
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	unsigned int value;
	unsigned int i;

	node = dir_node->internal;
	if(fat32_node_dir_entry(node, &dir))
		return 1;
	if(dir.size != 0)
		return 1;
	value = node->cluster_index;
	offset = calculated_offset(node);
	while((value & CHAIN_EOF) != CHAIN_EOF)
    {
    	readblock(offset + value - node->info.root_clstr_index, dirs);
    	for(i = 0; i < DIR_LEN; ++i)
		{
			if(!filename_cmp(&dirs[i], component_name))
			{
				new_vnode = vnode_create(dir_node->mount, v_ops, f_ops);
				new_vnode->internal = internal_node_create(node, (dirs[i].start_hi <<16) + dirs[i].start_lo, value, i);
				*target = new_vnode;
				return 0;
			}
		}
      	value = fat32_cluster_value(&node->info, value);
    }
  return 1;
}

static int create(vnode_t *dir_node, vnode_t **target,
	const char *component_name)
{
	fat32_node_t *node;
	directory_entry_t dir;

	// verify node type
	node = dir_node->internal;
	if(fat32_node_dir_entry(node, &dir))
		return 1;
	// directory check
	if(dir.size == 0)
		return 1;
	if(component_name[0] == '\0')
		return 1;
	*target = dir_node;
	return 1;
}

static fat32_node_t *internal_node_create(fat32_node_t *parent, unsigned int cluster_index, unsigned int dir_entry, unsigned int dir_index)
{
	fat32_node_t *new;
	new = (fat32_node_t*)kmalloc(sizeof(fat32_node_t));
	if(new == NULL)
		return NULL;
	new->info = parent->info;
	new->cluster_index = cluster_index;
	new->dir_entry = dir_entry;
	new->dir_index = dir_index;
	return new;
}

static int setup_mount(filesystem_t *fs, mount_t *mount)
{
	char buf[512];
	partition_entry_t *first_partition;
	boot_sector_t *boot_fat32;
	fat32_node_t init_node;
	// parse MBR to find first partition entry
	readblock(0, buf);
	first_partition = (partition_entry_t *) buf;
	init_node.info.logical_block_address = first_partition->logical_block_address;
	init_node.info.size = first_partition->size;
	readblock(init_node.info.logical_block_address, buf);
	boot_fat32 = (boot_sector_t *) buf;
	init_node.info.root_clstr_index = boot_fat32->root_clstr_index;
	init_node.info.count_of_reserved = boot_fat32->count_of_reserved;
	init_node.info.fat_num = boot_fat32->fat_num;
	init_node.info.sectors_num_per_fat = boot_fat32->sectors_num_per_fat;

	mount->fs = fs;
	mount->root = vnode_create(mount, v_ops, f_ops);
	mount->root->internal = internal_node_create(&init_node, init_node.info.root_clstr_index, 0, 0);
	if(mount->root->internal == NULL)
		return -1;
	return 0;
}

void fat32_ls(vnode_t *node){
	directory_entry_t dirs[DIR_LEN];
    fat32_node_t *fat32_node = node->internal;

    int blockIndex = fat32_node->info.logical_block_address + 
					fat32_node->info.count_of_reserved + 
					fat32_node->info.fat_num * 
					fat32_node->info.sectors_num_per_fat+
        			fat32_node->cluster_index -
        			fat32_node->info.root_clstr_index;
    readblock(blockIndex, dirs);

    for (int i = 0; i < DIR_LEN; i++) {
        printf("%s\n", dirs[i].name);
    }
}

void fat32_init()
{
    fat32_fs = (filesystem_t*)kmalloc(sizeof(filesystem_t));
	fat32_fs->name = "fat32";
	fat32_fs->setup_mount = setup_mount;
}