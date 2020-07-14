#include "fat32.h"

#define BLOCK_SIZE 512
#define CHAIN_LEN (BLOCK_SIZE / sizeof (unsigned int))
#define CHAIN_EOF 0xffffff8
#define DIR_LEN (BLOCK_SIZE / sizeof (struct directory_entry))


mount fat32_mount;
vnode fat32_root;
file_operations f_ops;
vnode_operations v_ops;

int offset_g=0;

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

                if (buf[i + j] < 32 || buf[i + j] > 126){
                    uart_send(" ");
                }
                else{
                    uart_send((char)buf[i + j]);
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
    // readblock (0, buf);
    // uart_puts("1\n");
    // part1 = (struct partition_entry *) buf;
    // init_node.info.lba = part1->lba;
    // init_node.info.size = part1->size;

    init_node.info.lba = 2048;
    // parse fat32 BPB
    readblock (init_node.info.lba, buf);
    fat32 = (void *) buf;
    init_node.info.cluster_num_of_root = fat32->cluster_num_of_root;
    init_node.info.count_of_reserved = fat32->count_of_reserved;
    init_node.info.num_of_fat = fat32->num_of_fat;
    init_node.info.sectors_per_fat = fat32->sectors_per_fat;
    fat32_root.internal = internal_node_create (&init_node, init_node.info.cluster_num_of_root, 0, 0);

    offset_g = init_node.info.lba + fat32->count_of_reserved + fat32->num_of_fat * fat32->sectors_per_fat;
    return 0;
}
int fat32_write (struct file_t *file, const void *buf, size_t len)
{
    uart_puts("fat32 write!\nTrying to write:\n```\n");
    uart_puts(buf);
    uart_puts("\n```\n");
    char wbuf[BLOCK_SIZE];
    struct fat32_node *node = file->vnode->internal;

    struct directory_entry dirs[DIR_LEN];
    readblock (offset_g + node->dir_entry - node->info.cluster_num_of_root, dirs);
    uart_puts(dirs[node->dir_index].name);
    uart_puts("\n");
    uart_send_int(dirs[node->dir_index].size);
    uart_puts("\n");

    int start_pos = dirs[node->dir_index].size;
    unsigned int offset = offset_g + node->cluster_index - node->info.cluster_num_of_root;    
    readblock(offset, wbuf);
    int i=0;
    for (i = 0; i < len; i++)
    {   
        if(((char*)buf)[i]=='\0')
            break;
        wbuf[start_pos + i] = ((char*)buf)[i];
    }
    dirs[node->dir_index].size = start_pos+i;
    writeblock (offset_g + node->dir_entry - node->info.cluster_num_of_root, dirs);
    writeblock (offset, wbuf);
    uart_puts(dirs[node->dir_index].name);
    uart_puts("\n");
    uart_send_int(dirs[node->dir_index].size);
    uart_puts("\n");

}

int fat32_read (struct file_t *file, void *buf, size_t len)
{
    uart_puts("fat32 read!\n");
    struct fat32_node *node = file->vnode->internal;
    unsigned int value, offset;
    char data[BLOCK_SIZE];
    offset = offset_g + node->cluster_index - node->info.cluster_num_of_root;
    readblock(offset, data);
    
    struct directory_entry dirs[DIR_LEN];
    readblock (offset_g + node->dir_entry - node->info.cluster_num_of_root, dirs);
    int file_size = dirs[node->dir_index].size;

    int i=0;
    for(i=0;i<len;i++){
        if(data[i]=='\0' || i>=file_size)
            break;
        ((char*)buf)[i] = data[i];
    }
    ((char*)buf)[i]='\0';
    return i;

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
    struct fat32_node *node;
    struct directory_entry dirs[DIR_LEN];

    node = dir_node->internal;

    unsigned int value = node->cluster_index;
    unsigned int offset = offset_g;
    readblock (offset+ node->cluster_index - node->info.cluster_num_of_root, dirs);
    for (int i = 0; i < DIR_LEN; ++i){
        // uart_puts(dirs[i].name);
        // uart_puts("\n");
        // uart_send_int(dirs[i].size);
        // uart_puts("\n");
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
    

    return 0;
}

int fat32_create (struct vnode_t *dir_node, struct vnode_t **target,
	const char *component_name)
{
}