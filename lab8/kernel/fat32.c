#include "vfs.h"
#include "sdhost.h"
#include "fat32.h"
#include "stdint.h"
#include "kernel.h"
#include "string.h"

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof(unsigned int))
#define CHAIN_EOF 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof(struct directory_entry))

extern struct mount *rootfs;
struct filesystem fs;
struct mount fat32;
int fat32_write(struct file *file, const void *buf, uint32_t len);
int fat32_read(struct file *file, void *buf, uint32_t len);
int fat32_create(struct vnode *dir_node, struct vnode **target,
		 const char *component_name);
int fat32_lookup(struct vnode *dir_node, struct vnode **target,
		 const char *component_name);

struct vnode_operations _v_ops = { .create = fat32_create,
				   .lookup = fat32_lookup };
struct file_operations _f_ops = { .read = fat32_read, .write = fat32_write };

struct vnode_operations *v_ops = &_v_ops;
struct file_operations *f_ops = &_f_ops;

void *fs_memcpy(void *dest, const void *src, int32_t len)
{
	char *d = dest;
	const char *s = src;
	while (len--)
		*d++ = *s++;
	return dest;
}

struct fat32_node node;
struct fat32_node *internal_node_create(struct fat32_node *parent,
					unsigned int cluster_index,
					unsigned int dir_entry,
					unsigned int dir_index)
{
	struct fat32_node *new = &node;
	new->info = parent->info;
	new->cluster_index = cluster_index;
	new->dir_entry = dir_entry;
	new->dir_index = dir_index;
	return new;
}

/* search for directory */
struct directory_entry dirs[DIR_LEN];
int fat32_node_dentry(struct fat32_node *node,
		      struct directory_entry *dir_entry)
{
	uint32_t offset;
	if (node->dir_index >= DIR_LEN) {
		return 1;
	}
	if (node->cluster_index == node->info.cluster_num_of_root) {
		dir_entry->size = 0;
		return 0;
	}
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	readblock(offset + node->dir_entry - node->info.cluster_num_of_root,
		  dirs);
	*dir_entry = dirs[node->dir_index];
}

uint32_t fat32_cluster_value(struct fat32_info *info, uint32_t index)
{
	uint32_t offset;
	uint32_t fat[CHAIN_LEN];
	offset = info->lba + info->count_of_reserved;
	offset += index / CHAIN_LEN;
	readblock(offset, fat);
	return fat[index % CHAIN_LEN];
}

int fat32_node_dentry_set(struct fat32_node *node,
			  struct directory_entry *dir_entry)
{
	struct directory_entry dirs[DIR_LEN];
	unsigned int offset;
	if (node->dir_index >= DIR_LEN)
		return 1;
	if (node->cluster_index == node->info.cluster_num_of_root) {
		dir_entry->size = 0;
		return 0;
	}
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	readblock(offset + node->dir_entry - node->info.cluster_num_of_root,
		  dirs);
	dirs[node->dir_index] = *dir_entry;
	writeblock(offset + node->dir_entry - node->info.cluster_num_of_root,
		   dirs);
	return 0;
}

int fat32_write(struct file *file, const void *buf, uint32_t len)
{
	struct fat32_node *node;
	struct directory_entry dir;
	uint32_t data_size;
	uint32_t valid_len, pos, start_pos, end_pos;
	uint32_t value, offset;
	char data[BLOCK_SIZE];
	node = file->vnode->internal;
	if (fat32_node_dentry(node, &dir) || dir.size == 0) {
		return -1;
	}
	data_size = dir.size / BLOCK_SIZE;
	if (dir.size % BLOCK_SIZE)
		data_size++;
	data_size *= BLOCK_SIZE;
	if (file->f_pos >= data_size)
		return 0;
	valid_len = (data_size - file->f_pos) > len ? len :
						      (data_size - file->f_pos);
	if (len < valid_len) {
		valid_len = len;
	}
	value == node->cluster_index;
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	pos = 0;
	while ((value & CHAIN_EOF) != CHAIN_EOF) {
		if (pos >= file->f_pos + valid_len)
			break;
		if (pos + BLOCK_SIZE > file->f_pos) {
			start_pos = (pos < file->f_pos) ? file->f_pos : pos;
			end_pos =
				(pos + BLOCK_SIZE) > (file->f_pos + valid_len) ?
					(file->f_pos + valid_len) :
					(pos + BLOCK_SIZE);
			readblock(offset + value -
					  node->info.cluster_num_of_root,
				  data);
			fs_memcpy(data + (start_pos % BLOCK_SIZE),
				  buf + (start_pos - file->f_pos),
				  end_pos - start_pos);
			writeblock(offset + value -
					   node->info.cluster_num_of_root,
				   data);
		}
		value = fat32_cluster_value(&node->info, value);
		pos += BLOCK_SIZE;
	}
	file->f_pos += valid_len;
	if (dir.size < file->f_pos) {
		dir.size = file->f_pos;
		fat32_node_dentry_set(node, &dir);
	}
	return valid_len;
}
int fat32_read(struct file *file, void *buf, uint32_t len)
{
	struct fat32_node *node;
	struct directory_entry dir;
	uint32_t valid_len, pos, start_pos, end_pos;
	uint32_t value, offset;
	char data[BLOCK_SIZE];
	node = file->vnode->internal;
	if (fat32_node_dentry(node, &dir) || dir.size == 0) {
		return -1;
	}
	/* EOF */
	if (file->f_pos >= dir.size) {
		return 0;
	}
	valid_len =
		(dir.size - file->f_pos) > len ? len : (dir.size - file->f_pos);
	value = node->cluster_index;
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	pos = 0;
	while ((value & CHAIN_EOF) != CHAIN_EOF) {
		if (pos >= file->f_pos + valid_len)
			break;
		if (pos + BLOCK_SIZE > file->f_pos) {
			start_pos = (pos < file->f_pos) ? file->f_pos : pos;
			end_pos =
				(pos + BLOCK_SIZE) > (file->f_pos + valid_len) ?
					(file->f_pos + valid_len) :
					BLOCK_SIZE;
			readblock(offset + value -
					  node->info.cluster_num_of_root,
				  data);
			fs_memcpy(buf + (start_pos - file->f_pos),
				  data + (start_pos % BLOCK_SIZE),
				  end_pos - start_pos);
		}
		value = fat32_cluster_value(&node->info, value);
		pos += BLOCK_SIZE;
	}
	file->f_pos += valid_len;
	return valid_len;
}
/* create new entry */
int fat32_create(struct vnode *dir_node, struct vnode **target,
		 const char *component_name)
{
	struct fat32_node *node = dir_node->internal;
	struct directory_entry dir;
	if (!fat32_node_dentry(node, &dir) && dir.size != 0) {
		*target = dir_node;
	}
	return 1;
}

struct fat32_node init_node;
struct vnode vnode_create;

void print_block(char *buf)
{
	for (int i = 0; i < 512; i++) {
		if (i % 16 == 0) {
			print("\n%x ", i);
		}
		print("%x ", buf[i]);
	}
}
uint32_t parse_int(char *buf)
{
	uint32_t res = 0;
	print("\n");
	for (int i = 0; i < 3; i++) {
		print("%d ", buf[i]);
		res |= buf[i] << ((3 - i) * 8);
	}
	pirnt("\n");
	return res;
}
int setup_mount(struct filesystem *fs, struct mount *mount)
{
	print("try mount");
	char buf[512];
	struct partition_entry *part1;
	struct boot_sector *fat32;
	print("first read\n");
	readblock(0, buf);
	print_block(buf);
	// part1 = (void *)buf;
	// init_node.info.lba = part1->lba;
	// init_node.info.size = part1->size;
	print("%d", parse_int(buf + 0x1c6));
	init_node.info.lba = buf[0x1c6];
	init_node.info.size = buf[0x1ca];
	print("done#1\n");
	print("second read\n");
	readblock(init_node.info.lba, buf);
	fat32 = (void *)buf;
	// init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
	// init_node.info.count_of_reserved = fat32->count_of_reserved;
	// init_node.info.num_of_fat = fat32->num_of_fat;
	// init_node.info.sectors_per_fat = fat32->sectors_per_fat;
	init_node.info.cluster_num_of_root = buf[0xe];
	init_node.info.count_of_reserved = buf[0x10];
	init_node.info.num_of_fat = buf[0x24];
	init_node.info.sectors_per_fat = buf[0x2c];
	print("done#2\n");

	mount->fs = fs;
	vnode_create.mount = &mount;
	// vnode_create.v_ops = v_ops;
	// vnode_create.f_ops = f_ops;
	print("done vnode create");
	mount->root = &vnode_create;
	print("all done");
	// print("h");
	// mount->root->mount = mount;
	// print("ha");
	// mount->root->v_ops = v_ops;
	// mount->root->f_ops = f_ops;
	if (mount->root == NULL)
		return 1;
	mount->root->internal = internal_node_create(
		&init_node, init_node.info.cluster_num_of_root, 0, 0);
	if (mount->root->internal == NULL)
		return 1;
	return 0;
}

int filename_exists(struct directory_entry *dir, const char *filename)
{
	int filename_len;
	/* only compare names*/
	for (filename_len = 0; filename_len < 8; filename_len++) {
		if (filename[filename_len] == '.') {
			break;
		}
		if (dir->name[filename_len] != filename[filename_len]) {
			return 0;
		}
	}
	for (int i = filename + 1; i < (filename + 4); i++) {
		if (filename[i] == '\0') {
			return dir->extension[i] == 0x20;
		}
		if (filename[i] != dir->extension[i])
			return 0;
	}
	return 1;
}
int fat32_lookup(struct vnode *dir_node, struct vnode **target,
		 const char *component_name)
{
	struct vnode *new_vnode;
	struct fat32_node *node;
	struct directory_entry dir;
	struct directory_entry dirs[DIR_LEN];
	uint32_t offset;
	uint32_t value;
	uint32_t i;
	node = dir_node->internal;
	if (fat32_node_dentry(node, &dir) || dir.size != 0) {
		return 1;
	}
	value = node->cluster_index;
	offset = node->info.lba + node->info.count_of_reserved;
	offset += node->info.num_of_fat * node->info.sectors_per_fat;
	while ((value & CHAIN_EOF) != CHAIN_EOF) {
		readblock(offset + value - node->info.cluster_num_of_root,
			  dirs);
		for (i = 0; i < DIR_LEN; i++) {
			if (!filename_exists(&dirs[i], component_name)) {
				new_vnode->mount = dir_node->mount;
				new_vnode->v_ops = v_ops;
				new_vnode->v_ops = f_ops;
				new_vnode->internal = internal_node_create(
					node,
					(dirs[i].start_hi << 16) +
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
void fat32_init()
{
	strcpy(fs.name, "fat32");
	fs.setup_mount = setup_mount;
	register_filesystem(&fs);
	setup_mount(&fs, rootfs);
}