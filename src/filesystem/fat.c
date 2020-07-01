#include "type.h"
#include "utils.h"
#include "device/uart.h"
#include "filesystem/filesystem.h"
#include "filesystem/sddriver.h"
#include "filesystem/fat.h"

uint32_t partition_start;
uint32_t root_sec;
struct fatBoot *boot_mbr;

void divideName(char *name, char *ext, char *org)
{
    bool space = false;
    size_t i, j;
    
    for (i = 0; i < 8; ++i)
    {
        if (*(org+i) == '.' || *(org+i) == '\0')
        {
            space = true;
            j = i;
        }
            
        if (space == true)
            *(name+i) = ' ';
        else
            *(name+i) = *(org+i);
    }

    for (i = 0; i < 3; ++i)
    {
        if (*(org+i+j) == '\0')
            break;
        *(ext+i) = *(org+i+j);
    }
    *(ext+i) = '\0';
}

void concateName(char *name, char *ext, char *ret)
{
    size_t i;
    for (i = 0; i < 8; ++i)
    {
        if (*(name+i) == '\0' || *(name+i) == ' ')
            break;

        *(ret+i) = *(name+i);
    }

    if ((*ext) == '\0' || (*ext) == ' ')
    {
        *(ret+i) = '\0';
        return;
    }
    else
    {
        *(ret+i) = '.';
        i++;
    }

    for (size_t j = 0; j < 3; ++j)
    {
        if (*(ext+j) == '\0')
            break;
        *(ret+i+j) = *(ext+j);
    }

    return;
}

void getPartition()
{
    unsigned char *buf = (unsigned char *)allocDynamic(512);
    readblock(0, buf);

    partition_start = *((uint32_t *)(buf+0x1C6));    
    readblock(partition_start, buf);

    boot_mbr = (struct fatBoot *)buf;
}

struct fatDentry *getRootDir()
{
    unsigned char *buf = (unsigned char *)allocDynamic(512);

    root_sec = partition_start + boot_mbr->fat32.sectors_per_fat * boot_mbr->fat_count + boot_mbr->reserved_sectors;
    readblock(root_sec, buf);

    struct fatDentry *dir = (struct fatDentry *)buf;

    return (struct fatDentry *)buf;
}

void fatmkvnode(struct fatDentry *fde, struct vnode *root)
{
    struct vnode *vn = (struct vnode *)allocSlot(root->mount->fs->vnode_token);
    vn->name = (char *)allocSlot(root->mount->fs->name_token);
    
    concateName(fde->name, fde->ext, vn->name);
    uartPuts("mkvnode: ");
    uartPuts(vn->name);
    uartPuts("\n");
    vn->mount = root->mount;
    vn->v_ops = root->v_ops;
    vn->f_ops = root->f_ops;
    vn->type = file;

    struct fcontent *fc = (struct fcontent *)allocSlot(root->mount->fs->fcontent_token);
    uint32_t cluster = ((uint32_t)fde->ch)<<16|fde->cl;
    // fc->content = (char *)allocDynamic(512);
    // readblock((cluster-2) * boot_mbr->sectors_per_cluster + root_sec, fc->content);
    
    fc->fsize = 512;
    vn->internal = fc;
    vn->node_info = fde;

    struct dentry *root_de = (struct dentry *)root->internal;
    root_de->child[root_de->child_num++] = vn;
}

uint32_t getChild(struct fatDentry *root_dir, struct vnode *vn)
{
    for(;root_dir->name[0]!=0;root_dir++) {
        if(root_dir->name[0]==0xE5 || root_dir->attr[0]==0xF) continue;

        fatmkvnode(root_dir, vn);
    }
    return 0;
}

struct fatDentry *getFreeDentry(struct fatDentry *root_dir)
{
    for(;root_dir->name[0]!=0;root_dir++) {
        if(root_dir->name[0]==0xE5 || root_dir->attr[0]==0xF) 
            continue;
    }

    return root_dir;
}

int32_t fatVnodeCreate(struct vnode *root, struct vnode **target, const char *pathname, vnode_t type)
{
    struct fatDentry *root_fde = (struct fatDentry *)root->node_info;

    struct fatDentry *fde = getFreeDentry(root_fde);

    divideName(fde->name, fde->ext, pathname);
    fde->size = 512;
    fde->attr[0] = 0x10;
    fde->cl = 1000;
    
    fatmkvnode(fde, root);
    writeblock(root_sec, root_fde);
}

int32_t fatVnodeLookup(struct vnode *root, struct vnode **target, const char *pathname)
{
    struct dentry *de = (struct dentry *)root->internal;

    for (int32_t i = 0, end = de->child_num; i < end; ++i)
    {
        if (strcmp(de->child[i]->name, pathname))
        {   
            uartPuts("match: ");
            uartPuts(de->child[i]->name);
            uartPuts("\n");

            *target = de->child[i];
            return 0;
        }
    }

    return -1;
}

int32_t fatListDir(struct vnode *root, const char *pathname)
{

}

int32_t fatFileWrite(struct file *file, const void *buf, size_t len)
{
    char *file_content;
    char *cur_pos;
    size_t written;
    struct fatDentry *fde = (struct fatDentry *)file->vnode->node_info;
    uint32_t cluster = ((uint32_t)fde->ch)<<16|fde->cl;

    if (file->f_pos + len >= 512)
        return 0;

    file_content = allocDynamic(512);
    readblock((cluster-2) * boot_mbr->sectors_per_cluster + root_sec, file_content);
    cur_pos = file_content + file->f_pos;

    written = copynstr((char *)buf, cur_pos, len);
    writeblock((cluster-2) * boot_mbr->sectors_per_cluster + root_sec, file_content);
    file->f_pos += written;

    if (file->f_pos > fde->size)
    {
        struct fatDentry *root_fde = (struct fatDentry *)file->vnode->mount->root->node_info;
        fde->size = file->f_pos;
        writeblock(root_sec, root_fde);
    }

    freeDynamic(file_content);

    return written;
}

int32_t fatFileRead(struct file *file, void *buf, size_t len)
{
    char *file_content;
    char *cur_pos;
    size_t read;
    struct fatDentry *fde = (struct fatDentry *)file->vnode->node_info;
    uint32_t cluster = ((uint32_t)fde->ch)<<16|fde->cl;

    file_content = allocDynamic(512);
    readblock((cluster-2) * boot_mbr->sectors_per_cluster + root_sec, file_content);
    cur_pos = file_content + file->f_pos;
    read = copynstr(cur_pos, (char *)buf, len);
    file->f_pos += read;

    freeDynamic(file_content);

    return read;
}

int32_t fatSetupMount(struct filesystem* fs, struct mount* mount)
{
    sd_init();
    getPartition();

    struct vnode *vn = (struct vnode *)allocSlot(fs->vnode_token);
    vn->name = (char *)allocSlot(fs->name_token);
    copynstr("/", vn->name, NAME_BUFFER_SIZE);
    vn->mount = mount;
    vn->v_ops = (struct vnodeOperations *)allocDynamic(sizeof(struct vnodeOperations));
    vn->v_ops->create = fatVnodeCreate;
    vn->v_ops->lookup = fatVnodeLookup;
    vn->v_ops->listdir = fatListDir;
    vn->f_ops = (struct fileOperations *)allocDynamic(sizeof(struct fileOperations));
    vn->f_ops->write = fatFileWrite;
    vn->f_ops->read = fatFileRead;
    vn->type = dir;

    fs->dentry_token = getFreePool(sizeof(struct dentry));
    fs->fcontent_token = getFreePool(sizeof(struct fcontent));

    struct dentry *de = (struct dentry *)allocSlot(fs->dentry_token);
    de->vnode = vn;
    de->child_num = 0;
    de->child = (struct dentry **)allocDynamic(sizeof(struct dentry *) * MAX_DIR_CHILD);
    vn->internal = de;

    struct fatDentry *fde;
    fde = getRootDir();

    mount->fs = fs;
    mount->root = vn;

    getChild(fde, vn); 
    vn->node_info = fde;

    return 0;
}