#include "uart.h"
#include "str.h"
#include "mailbox.h"
#include "lfb.h"
#include "exc.h"
#include "syscall.h"
#include "tmpfs.h"
#include "buddy.h"
#include "fat32.h"

extern struct mount* rootfs;

void mount_otherfs(struct mount* otherfs)
{
	struct vnode* root_vnode = rootfs->root;
	int idx = root_vnode->num_of_child;
	root_vnode->child[idx] = (struct dentry*)varied_allocate(sizeof(struct dentry));
	root_vnode->child[idx]->name = "MYFAT";
	root_vnode->child[idx]->vnode = otherfs->root;
	root_vnode->child[idx]->parent = root_vnode;
	root_vnode->num_of_child++;
}

void fat32_testcase()
{
	char buff[50]={0};
	struct file* a = vfs_open("/MYFAT/DDIIRR/HELLO", 0);
	//my_printf("file: %s",buff);
	//vfs_write(a, "ZZZZ", 4);
	vfs_read(a, buff, 50);
	my_printf("file: %s",buff);
	//uart_hex(a);
}

void main()
{
    int sector_size, cluster_sectors, root_dir_sector, num_of_reserved_sec, FATs_secs, num_of_FATs;
    uart_init();
    link_init();
    rootfs_init();
    sd_init();
    struct mount* fat32_root_vnode = fat32_init();
	
	mount_otherfs(fat32_root_vnode);
	//my_printf("%d", (my_strcmp("AAA", "AAB", 2)));
	fat32_testcase();

	while(1);
	
}
