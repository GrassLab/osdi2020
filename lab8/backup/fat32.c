#include "sd.h"
#include "vfs.h"
#include "fat32.h"
#include "mm.h"

static int write(file_t *file, const void *buf, size_t len);
static int read(file_t *file, void *buf, size_t len);
static int create(vnode_t *dir_node, vnode_t **target,
		   const char *component_name);
static int lookup(vnode_t *dir_node, vnode_t **target,
		   const char *component_name);
static fat32_node_t *internal_node_create(fat32_node_t *parent,
						unsigned int cluster_index,
						unsigned int dir_entry,
						unsigned int dir_index);

static vnode_operations_t _v_ops = {.create = create,.lookup = lookup };
static file_operations_t _f_ops = {.read = read,.write = write };

static vnode_operations_t *v_ops = &_v_ops;
static file_operations_t *f_ops = &_f_ops;

// TODO: cache
static unsigned int fat32_cluster_value(fat32_info_t *info, unsigned int index)
{
	unsigned int offset;
	unsigned int fat[CHAIN_LEN];

  // find FAT
	offset = info->lba + info->count_of_reserved;
	offset += index / CHAIN_LEN;
	readblock(offset, fat);
	return fat[index % CHAIN_LEN];
}

//using fat32 node to get dir entry
static int fat32_node_dir_entry(fat32_node_t *node, directory_entry_t *dir_entry)
{
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	if(node->dir_index >= DIR_LEN)
		return 1;
	if(node->cluster_index == node->info.cluster_num_of_root)
	{
		dir_entry->size = 0;
		return 0;
	}
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	readblock(offset + node->dir_entry - node->info.cluster_num_of_root, dirs);
	*dir_entry = dirs[node->dir_index];
	return 0;
}

static int fat32_node_dir_entry_set(fat32_node_t *node, directory_entry_t *dir_entry)
{
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	if(node->dir_index >= DIR_LEN)
		return 1;
	if(node->cluster_index == node->info.cluster_num_of_root)
	{
		dir_entry->size = 0;
		return 0;
	}
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	readblock(offset + node->dir_entry - node->info.cluster_num_of_root, dirs);
	dirs[node->dir_index] = *dir_entry;
	writeblock(offset + node->dir_entry - node->info.cluster_num_of_root,
			dirs);
	return 0;
}

static int write(file_t *file, const void *buf, size_t len)
{
    fat32_node_t *node = file->vnode->internal;
	directory_entry_t dir;
	size_t valid_len, pos, start_pos, end_pos;
	unsigned int value, offset;
	unsigned int DATA_SIZE;
	char data[BLOCK_SIZE];
	// type error
	if(fat32_node_dir_entry(node, &dir))
		return -1;
	if(dir.size == 0)
		return -1;
	// TODO: find free cluster for new data
	// block full
	DATA_SIZE = dir.size / BLOCK_SIZE;
	if(dir.size % BLOCK_SIZE)
		DATA_SIZE++;
	DATA_SIZE *= BLOCK_SIZE;
	if(file->f_pos >= DATA_SIZE)
		return 0;
	valid_len = DATA_SIZE - file->f_pos;
	if(len < valid_len)
		valid_len = len;
	// traversal all block to find target
	// find block offset of fat cluster chain
	value = node->cluster_index;
	// find offset of data region
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
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
			readblock(offset + value - node->info.cluster_num_of_root, data);
			// modify block
			_memcpy(buf +(start_pos - file->f_pos), 
					data +(start_pos % BLOCK_SIZE),
					end_pos - start_pos);
			// write back
			writeblock(offset + value - node->info.cluster_num_of_root, data);
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
	size_t valid_len, pos, start_pos, end_pos;
	unsigned int value, offset;
	char data[BLOCK_SIZE];
	//using internal to get fat32_node
	node = file->vnode->internal;
	// type error
	if(fat32_node_dir_entry(node, &dir))
		return -1;
	if(dir.size == 0)
		return -1;
	// f_pos at EOF
	if(file->f_pos >= dir.size)
		return 0;
	valid_len = dir.size - file->f_pos;
	if(len < valid_len)
		valid_len = len;
	// traversal all block to find target
	// find block offset of fat cluster chain
	value = node->cluster_index;
	// find offset of data region
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	pos = 0;
	while((value & CHAIN_EOF) != CHAIN_EOF)
	{
		if(pos >= file->f_pos + valid_len)
			break;
		if(pos + BLOCK_SIZE > file->f_pos)
		{
			start_pos = (pos < file->f_pos) ? file->f_pos : pos;
			end_pos = pos + BLOCK_SIZE;
			if(end_pos > file->f_pos + valid_len)
			end_pos = file->f_pos + valid_len;
			// read block
			readblock(offset + value - node->info.cluster_num_of_root, data);
			// write to user buffer
			// from , dst
			for(int i = 0; i < 100; i++){
				printf("%c\n", data[i]);
			}
			printf("content data is %s\n", data +(start_pos % BLOCK_SIZE));

			_memcpy(data +(start_pos % BLOCK_SIZE), 
				buf +(start_pos - file->f_pos),
				 end_pos - start_pos);
		}

		value = fat32_cluster_value(&node->info, value);
		pos += BLOCK_SIZE;
	}
	file->f_pos += valid_len;
	return valid_len;
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

static int lookup(vnode_t *dir_node, vnode_t **target, const char *component_name)
{
	vnode_t *new_vnode;
	fat32_node_t *node;
	directory_entry_t dir;
	directory_entry_t dirs[DIR_LEN];
	unsigned int offset;
	unsigned int value;
	unsigned int i;

    // verify node type
	node = dir_node->internal;
	if(fat32_node_dir_entry(node, &dir))
		return 1;
	// directory check
	if(dir.size != 0)
		return 1;
	// traversal all block to find target
	// find block offset of fat cluster chain
	value = node->cluster_index;
	// find offset of data region
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	while((value & CHAIN_EOF) != CHAIN_EOF)
    {
      // read block
    	readblock(offset + value - node->info.cluster_num_of_root, dirs);
      // search component
    	for(i = 0; i < DIR_LEN; ++i)
		{
			if(!filename_cmp(&dirs[i], component_name))
			{
			// TODO: memory leak
				new_vnode = vnode_create(dir_node->mount, v_ops, f_ops);
				new_vnode->internal = internal_node_create(node,
									(dirs[i].start_hi <<
									16) +
									dirs[i].start_lo,
									value, i);
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

static fat32_node_t *
internal_node_create(fat32_node_t *parent, unsigned int cluster_index, unsigned int dir_entry, unsigned int dir_index)
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
	// TODO: get device and fat32 offset by parameter
	char buf[512];
	partition_entry_t *part1;
	boot_sector_t *fat32;
	fat32_node_t init_node;
	// parse MBR to find first partition entry
	readblock(0, buf);
	part1 = (void *) buf;
	init_node.info.lba = part1->lba;
	init_node.info.size = part1->size;
	// parse fat32 BPB
	readblock(init_node.info.lba, buf);
	fat32 = (void *) buf;
	init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
	init_node.info.count_of_reserved = fat32->count_of_reserved;
	init_node.info.num_of_fat = fat32->num_of_fat;
	init_node.info.sectors_per_fat = fat32->sectors_per_fat;

	mount->fs = fs;
	mount->root = vnode_create(mount, v_ops, f_ops);
	if(mount->root == NULL)
		return 1;
	mount->root->internal = internal_node_create(&init_node, init_node.info.cluster_num_of_root, 0, 0);
	if(mount->root->internal == NULL)
		return 1;
	return 0;
}

int fat32_ls(vnode_t *node){
	directory_entry_t dirs[DIR_LEN];
    fat32_node_t *fat32_node = node->internal;

    int blockIndex = 
        fat32_node->info.lba +
        fat32_node->info.count_of_reserved +
        fat32_node->info.num_of_fat * fat32_node->info.sectors_per_fat;
        // fat32_node->cluster_index -
        // fat32_node->info.cluster_num_of_root;
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