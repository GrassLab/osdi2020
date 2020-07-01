#include "fat32.h"

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof (unsigned int))
#define CHAIN_EOF 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof (struct directory_entry))


mount fat32_mount;
vnode fat32_root;
file_operations f_ops;
vnode_operations v_ops;

void print_block(int block)
{
    char buf[512];
    readblock(block, &buf);

    for (int i = 0; i < 512; i += 16)
    {
        uart_hex(i + block * 512);
        uart_puts(": ");
        for (int j = 0; j < 16; j += 1)
        {
            // printf("%02x", (buf[i + j]));
            uart_hex((buf[i + j]));
            uart_puts(" ");
            
        }
        uart_send('\n');
        for (int j = 0; j < 16; j++)
        {
            switch (buf[i + j])
            {
            case '\n':
                uart_puts("\\n");
                break;
            case '\r':
                uart_puts("\\r");
                break;
            case 0:
                uart_puts(".");
                break;
            default:
                if (buf[i + j] < 32)
                    uart_puts(" ");
                else{
                    uart_puts("*");
                }                    
            }
        }
        uart_puts("\n\r");
    }
    uart_puts("===\n\r");
}

struct fat32_node * internal_node_create (struct fat32_node *parent, unsigned int cluster_index,
		      unsigned int dir_entry, unsigned int dir_index)
{
    struct fat32_node *new;
    new = var_alloc (sizeof (*new));
    if (new == NULL)
        return NULL;
    new->info = parent->info;
    new->cluster_index = cluster_index;
    new->dir_entry = dir_entry;
    new->dir_index = dir_index;
    return new;
}

unsigned int fat32_cluster_value (struct fat32_info *info, unsigned int index)
{
  unsigned int offset;
  unsigned int fat[CHAIN_LEN];

  // find FAT
  offset = info->lba + info->count_of_reserved;
  offset += index / CHAIN_LEN;
  readblock (offset, fat);
  return fat[index % CHAIN_LEN];
}

filesystem fat32_filesystem(){
    filesystem fs;
    fs.name = "fat32";
    fs.setup_mount = fat32_setup_mount;
    return fs;
}

int fat32_setup_mount(struct filesystem_t *fs, struct mount_t **mount){
    uart_puts("fat32 setup\n");
    struct partition_entry *part1;
    struct boot_sector *fat32;
    struct fat32_node init_node;
    fat32_mount.root = &fat32_root;
    fat32_mount.fs = fs;

    *mount = &fat32_mount;

    f_ops.write = fat32_write;
    f_ops.read = fat32_read;

    v_ops.lookup = fat32_lookup;
    v_ops.create = fat32_create;

    fat32_root.mount = &fat32_mount;
    fat32_root.v_ops = &v_ops;
    fat32_root.f_ops = &f_ops;

    char buf[512];
    

    // parse MBR to find first partition entry
    readblock (0, buf);
    // print_block(0);
    part1 = (void *) buf;
    init_node.info.lba = part1->lba;
    init_node.info.size = part1->size;
    
    // parse fat32 BPB
    readblock (init_node.info.lba, buf);
    // print_block(init_node.info.lba);
    fat32 = (void *) buf;
    init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
    init_node.info.count_of_reserved = fat32->count_of_reserved;
    init_node.info.num_of_fat = fat32->num_of_fat;
    init_node.info.sectors_per_fat = fat32->sectors_per_fat;

    fat32_root.internal = internal_node_create (&init_node, init_node.info.cluster_num_of_root, 0, 0);

    return 0;
}
int fat32_write (struct file_t *file, const void *buf, size_t len)
{
}

int fat32_read (struct file_t *file, void *buf, size_t len)
{
}

char lower2upper(char s){
    if(s >= 'a' && s <= 'z') {
        s = s -32;
    }
    return s;
}

int compare_file(char * search_name, char * search_ext, char * target){
    int name_index, ext_index;
    for(name_index=0;name_index<8;name_index++){
        if(target[name_index]=='.'){            
            break;
        }        
        if(lower2upper(target[name_index]) != search_name[name_index]){
            return 0;
        }
    }
    target = target+name_index+1;
    for(ext_index=0;ext_index<3;ext_index++){
        if(target[ext_index]=='\0'){            
            break;
        }
        if(lower2upper(target[ext_index]) != search_ext[ext_index]){
            return 0;
        }
    }
    return 1;
}

int fat32_lookup (struct vnode_t *dir_node, struct vnode_t **target,
	const char *component_name)
{
    uart_puts("fat32 lookup!\nlookup for file: ");
    uart_puts(component_name);
    uart_puts("\n");
    struct vnode *new_vnode;
    struct fat32_node *node;
    struct directory_entry dir;
    struct directory_entry dirs[DIR_LEN];
    unsigned int offset;
    unsigned int value;
    unsigned int i;

    node = dir_node->internal;

    value = node->cluster_index;
    offset = node->info.lba + node->info.count_of_reserved + node->info.num_of_fat * node->info.sectors_per_fat;

    
    while((value & CHAIN_EOF) != CHAIN_EOF){
        readblock (offset + value - node->info.cluster_num_of_root, dirs);
        for (i = 0; i < DIR_LEN; ++i){
            // compare filename
            if(compare_file(dirs[i].name, dirs[i].extension, component_name)){
                struct vnode_t *new;
                new = var_alloc (sizeof (*new));
                new->mount = &dir_node->mount;
                new->v_ops = &v_ops;
                new->f_ops = &f_ops;
                new->internal = internal_node_create (node,(dirs[i].start_hi <<16) +dirs[i].start_lo,value, i);
                *target = new;
	            return 1;
            }            

        }
        value = fat32_cluster_value (&node->info, value);
    }

    return 0;
}

int fat32_create (struct vnode_t *dir_node, struct vnode_t **target,
	const char *component_name)
{
}