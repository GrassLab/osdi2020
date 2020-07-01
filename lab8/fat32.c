#include "vfs.h"
#include "sdhost.h"
#include "fat32.h"
#include "str.h"






void print_block(char *block)
{
	for(int i=0;i<512;i++)
	{
		if(i%16==0){
			uart_puts("\r\n");
			my_printf("%x : ", i);
		}
		my_printf("%x ", block[i]);
	}
	uart_puts("\r\n");
}

int fat32_vnode_init(struct mount*mount, struct vnode* root_vnode)
{
    root_vnode->v_ops = (struct vnode_operations*)varied_allocate(sizeof(struct vnode_operations)); 
    root_vnode->f_ops = (struct file_operations*)varied_allocate(sizeof(struct file_operations));
    root_vnode->mount = mount;
    root_vnode->num_of_child = 0;
    for(int i=0;i<DENTRY_MAX_CHILD;i++)
    {
        root_vnode->child[i] = 0;
    }
    root_vnode->v_ops->lookup = fat32_vnode_lookup;
    root_vnode->v_ops->create = fat32_vnode_creat;
    root_vnode->f_ops->write = fat32_write;
    root_vnode->f_ops->read = fat32_read;
    return 1;
}



void create_all_file_vnode(struct vnode* root_vnode, int root_dir_idx, struct mount* mount)
{
    char buf[512],buf2[512];
    struct vnode *tmp_vnode;
    readblock(root_dir_idx, buf);
    //print_block(buf);
    for(int i=0;i<0x200;i+=0x20)
    {
        if(buf[i]==0) break;
        else if(buf[i]==0xE5) continue;
        else if(buf[i]==0x2E) continue;
        if(buf[i+0xb] == FAT32_FILE)
        {
            root_vnode->child[root_vnode->num_of_child] = (struct dentry*)varied_allocate(sizeof(struct dentry));
            

            tmp_vnode = (struct vnode*)varied_allocate(sizeof(struct vnode));
            fat32_vnode_init(mount, tmp_vnode);
            
            tmp_vnode->internal = (FAT32fs*)varied_allocate(sizeof(FAT32fs));
            ((FAT32fs *)tmp_vnode->internal)->meta = ((FAT32fs *)root_vnode->internal)->meta;
            ((FAT32fs *)tmp_vnode->internal)->file_type = FAT32_FILE;
            ((FAT32fs *)tmp_vnode->internal)->file_cluster_idx = (byte_to_int(buf+i+0x14,2)<<64) + byte_to_int(buf+i+0x1a,2);
            ((FAT32fs *)tmp_vnode->internal)->file_size = (byte_to_int(buf+i+0x1c,4));
            //my_printf("%d,", ((FAT32fs *)tmp_vnode->internal)->file_size);
            my_strcpy(((FAT32fs *)tmp_vnode->internal)->name, buf+i, 8);
            my_strcpy(((FAT32fs *)tmp_vnode->internal)->ext, buf+i+8, 3);
            mystr_strip(((FAT32fs *)tmp_vnode->internal)->name);
            mystr_strip(((FAT32fs *)tmp_vnode->internal)->ext);
            root_vnode->child[root_vnode->num_of_child]->name = ((FAT32fs *)tmp_vnode->internal)->name;
            //my_printf("%d : %s\r\n",  root_vnode->num_of_child,root_vnode->child[root_vnode->num_of_child]->name);
            root_vnode->child[root_vnode->num_of_child]->vnode = tmp_vnode;
            root_vnode->child[root_vnode->num_of_child]->parent = root_vnode;
            root_vnode->num_of_child +=1;
            //my_printf("clu idx: %d, name: %s, ext: %s, file_type: %x\r\n", ((FAT32fs *)tmp_vnode->internal)->file_cluster_idx, ((FAT32fs *)tmp_vnode->internal)->name, ((FAT32fs *)tmp_vnode->internal)->ext, ((FAT32fs *)tmp_vnode->internal)->file_type);
        }
        else if(buf[i+0xb] == FAT32_DIR)
        {
            root_vnode->child[root_vnode->num_of_child] = (struct dentry*)varied_allocate(sizeof(struct dentry));
            

            tmp_vnode = (struct vnode*)varied_allocate(sizeof(struct vnode));
            fat32_vnode_init(mount, tmp_vnode);

            tmp_vnode->internal = (FAT32fs*)varied_allocate(sizeof(FAT32fs));
            ((FAT32fs *)tmp_vnode->internal)->meta = ((FAT32fs *)root_vnode->internal)->meta;
            ((FAT32fs *)tmp_vnode->internal)->file_type = FAT32_DIR;
            ((FAT32fs *)tmp_vnode->internal)->file_cluster_idx = (byte_to_int(buf+i+0x14,2)<<64) + byte_to_int(buf+i+0x1a,2);
            my_strcpy(((FAT32fs *)tmp_vnode->internal)->name, buf+i, 8);
            my_strcpy(((FAT32fs *)tmp_vnode->internal)->ext, buf+i+8, 3);
            mystr_strip(((FAT32fs *)tmp_vnode->internal)->name);
            mystr_strip(((FAT32fs *)tmp_vnode->internal)->ext);
            ((FAT32fs *)tmp_vnode->internal)->file_size = (byte_to_int(buf+i+0x1c,4));
            root_vnode->child[root_vnode->num_of_child]->name = ((FAT32fs *)tmp_vnode->internal)->name;
            root_vnode->child[root_vnode->num_of_child]->vnode = tmp_vnode;
            root_vnode->child[root_vnode->num_of_child]->parent = root_vnode;
            create_all_file_vnode(tmp_vnode, ((FAT32fs *)root_vnode->internal)->meta->root_dir_start_idx+((FAT32fs *)tmp_vnode->internal)->file_cluster_idx - 2, mount);
            root_vnode->num_of_child +=1;
            //my_printf("%d\r\n", ((FAT32fs *)tmp_vnode->internal)->file_cluster_idx);
            //readblock(((FAT32fs *)root_vnode->internal)->meta->root_dir_start_idx+((FAT32fs *)tmp_vnode->internal)->file_cluster_idx - 2, buf2);
            //print_block(buf2);
        }
        else
        {
            continue;
        }
    }
}




void fat32_setup_mount(struct filesystem* fs, struct mount* mount)
{
    FAT32fs *metadata = (FAT32fs*)varied_allocate(sizeof(FAT32fs));
    metadata->meta = (FAT32_info*)varied_allocate(sizeof(FAT32_info));
    char buf[512];
    readblock(0, buf);
    metadata->meta->first_block_idx = byte_to_int(buf+454, 4);
    metadata->meta->size = byte_to_int(buf+458, 4);
    //my_printf("Partition type: %x\r\nFirst Logical block address: %d\r\n",  metadata->meta->size, metadata->meta->first_block_idx);
    readblock(metadata->meta->first_block_idx, buf);
    metadata->meta->sector_size = byte_to_int(buf+0x0B,2);
	metadata->meta->cluster_sectors = byte_to_int(buf+0x0D,1);
	metadata->meta->num_of_reserved_sec =  byte_to_int(buf+0x0E,2);
	metadata->meta->num_of_FATs = byte_to_int(buf+0x10,1);
	metadata->meta->FATs_secs = byte_to_int(buf+0x24,4);
	metadata->meta->root_dir_sector = byte_to_int(buf+0x2C,4);
    metadata->meta->root_dir_start_idx = metadata->meta->first_block_idx + metadata->meta->num_of_reserved_sec + metadata->meta->FATs_secs*metadata->meta->num_of_FATs;
    metadata->meta->FAT_cluster_start_idx = metadata->meta->first_block_idx + metadata->meta->num_of_reserved_sec;
    //my_printf("Sector size : %d\r\n\
Num of sectors per cluster : %d\r\n\
Count of reserved logical sectors: %d\r\n\
Number of File Allocation Tables. Almost always 2: %d\r\n\
Logical sectors per file allocation table : %d\r\n\
Cluster number of root directory start : %d\r\n"\
	, metadata->meta->sector_size, metadata->meta->cluster_sectors, metadata->meta->num_of_reserved_sec, metadata->meta->num_of_FATs, metadata->meta->FATs_secs, metadata->meta->root_dir_sector);
    // cluster -2
    readblock(metadata->meta->root_dir_start_idx, buf);
    //readblock(metadata->meta.first_block_idx + metadata->meta.num_of_reserved_sec, buf);
    //print_block(buf);
    //while(1);
    //uart_hex(buf[100]);
    struct vnode *root_vnode = (struct vnode*)varied_allocate(sizeof(struct vnode));
    //parse_file_allocation_table()
    root_vnode->internal = metadata;
    fat32_vnode_init(mount, root_vnode);
    create_all_file_vnode(root_vnode, metadata->meta->root_dir_start_idx, mount);
    //uart_puts(root_vnode->child[4]->vnode->child[0]->name);
    mount->fs = fs;
    mount->root = root_vnode;
}


struct mount * fat32_init()
{
    struct filesystem *fat32_file_sys = (struct filesystem*)varied_allocate(sizeof(struct filesystem));
    struct mount *tmpfat32 = (struct mount*)varied_allocate(sizeof(struct mount));
    fat32_file_sys->name = "fat32";
    register_filesystem(fat32_file_sys);
    fat32_file_sys->setup_mount(fat32_file_sys, tmpfat32);
    return tmpfat32;
}

int fat32_vnode_lookup(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
  for(int i=0;i<dir_node->num_of_child;i++)
  {
    if( my_strcmp(dir_node->child[i]->name, component_name, my_strlen(component_name)) == 0 )
    {
      *target = dir_node->child[i]->vnode;
      return 1;
    }
  }
  return 0;
}

int fat32_vnode_creat(struct vnode* dir_node, struct vnode** target, const char* component_name)
{
    //TO_DO
}

int fat32_read(struct file* file, void* buf, int len)
{
    FAT32fs *metadata = file->vnode->internal;
    int cluster_base = metadata->meta->root_dir_start_idx -2;
    char cbuf[512];
    //my_printf("%d\r\n",metadata->file_cluster_idx);
    if(len>512)
    {
        int num_iter = len/512;
        int num_block, offset, findidx;
        num_iter = (num_iter%512==0 ? num_iter:num_iter+1);
        int *indexlist = (int*)varied_allocate(sizeof(int)*num_iter);
        indexlist[0] = metadata->file_cluster_idx;
        findidx = metadata->file_cluster_idx;
        for(int i=1;i<num_iter;i++)
        {
            num_block = (findidx/128);
            offset = (findidx%128);
            readblock(metadata->meta->FAT_cluster_start_idx+num_block, cbuf);
            indexlist[i] = sp_byte_to_int(cbuf+offset*4);
            if(indexlist[i] == FILE_EOF)
            {
                num_iter = i;
                break;
            }
            findidx = indexlist[i];
        }
        for(int i=0;i<num_iter;i++)
        {
            readblock(cluster_base+indexlist[i], cbuf);
            print_block(cbuf);
        }
    }
    else{
        readblock(cluster_base + metadata->file_cluster_idx, cbuf);
        my_strcpy(buf, cbuf, len);
    }
    return 1;
}
char FAT32_write_buf[512]={0};
int fat32_write(struct file* file, const void* buf, int len)
{
    uart_puts("AAAAAA\r\n");
    if(len>512) return 0;
    FAT32fs *metadata = file->vnode->internal;
    int cluster_base = metadata->meta->root_dir_start_idx -2;
    my_strcpy(FAT32_write_buf, buf, len);
    writeblock(cluster_base + metadata->file_cluster_idx, FAT32_write_buf);
    //print_block(FAT32_write_buf);
}