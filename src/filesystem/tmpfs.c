#include "utils.h"
#include "memory/memPool.h"
#include "filesystem/filesystem.h"
#include "filesystem/tmpfs.h"
#include "device/uart.h"

int32_t checkPath(const char *pathname)
{
    if (*pathname == '\0')
        return -1;

    return 0;
}

int32_t __vnodeLookup(struct vnode **target, const char *component)
{
    struct dentry *de = (struct dentry *)(*target)->internal;

    uartPuts("lookup: ");
    uartPuts(component);
    uartPuts("\n");

    if (strcmp(component, "/"))
    {
        return 0;
    }

    for (int32_t i = 0, end = de->child_num; i < end; ++i)
    {
        if (strcmp(de->child[i]->name, component))
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

int32_t mkvnode(struct vnode *parent, struct vnode **target, const char *name, vnode_t type)
{
    struct vnode *vn = (struct vnode *)allocSlot(parent->mount->fs->vnode_token);
    vn->name = (char *)allocSlot(parent->mount->fs->vnode_token);
    copynstr(name, vn->name, NAME_BUFFER_SIZE);

    uartPuts("mkvnode: ");
    uartPuts(name);
    uartPuts("\n");

    vn->mount = parent->mount;
    vn->v_ops = parent->v_ops;
    vn->f_ops = parent->f_ops;
    vn->type = type;

    if (type == dir)
    {
        struct dentry *de = (struct dentry *)allocSlot(parent->mount->fs->dentry_token);
        de->vnode = vn;
        de->child_num = 0;
        de->child = (struct dentry **)allocDynamic(sizeof(struct dentry *) * MAX_DIR_CHILD);
        vn->internal = de;
    }  
    else if (type == file)
    {
        struct fcontent *fc = (struct fcontent *)allocSlot(parent->mount->fs->fcontent_token);
        fc->content = (char *)allocDynamic(1024);
        fc->fsize = 1024;
        vn->internal = fc;
    }

    struct dentry *parent_de = (struct dentry *)parent->internal;
    parent_de->child[parent_de->child_num++] = vn;

    *target = vn;

    return 0;
}

int32_t tmpfsVnodeCreate(struct vnode *root, struct vnode **target, const char *pathname, vnode_t type)
{
    if (checkPath(pathname) != 0)
        return -1;

    char component[NAME_BUFFER_SIZE];
    char *p, *cur;
    copynstr(pathname, component, NAME_BUFFER_SIZE); 
    p = component + 1;
    cur = component;

    uartPuts("tmpfs create: ");
    uartPuts(pathname);
    uartPuts("\n");

    *target = root;

    while(1)
    {
        if (*p == '\0')
        {
            int32_t err = __vnodeLookup(target, cur);

            if (err == -1)
                mkvnode(*target, target, cur, type);
            else if ((*target)->type != type)
                return -1; 

            break;
        }

        if (*p == '/')
        {
            *p = '\0';

            int32_t err = __vnodeLookup(target, cur);
            if (err == -1)
                mkvnode(*target, target, cur, dir);
            else if ((*target)->type != dir)
                return -1;

            cur = p + 1;
        }

        p++;
    }

    return 0;
}

int32_t tmpfsVnodeLookup(struct vnode *root, struct vnode **target, const char *pathname)
{
    if (checkPath(pathname) != 0)
        return -1;

    char component[NAME_BUFFER_SIZE];
    char *p, *cur;
    copynstr(pathname, component, NAME_BUFFER_SIZE); 
    p = component + 1;
    cur = component;

    *target = root;

    uartPuts("tmpfs lookup: ");
    uartPuts(pathname);
    uartPuts("\n");

    while(1)
    {
        if (*p == '\0')
        {
            int32_t err = __vnodeLookup(target, cur);
            if (err != 0)
                return err;

            break;
        }

        if (*p == '/')
        {
            *p = '\0';

            int32_t err = __vnodeLookup(target, cur);
            if (err != 0)
                return err;

            cur = p + 1;
        }

        p++;
    }

    return 0;
}

int32_t tmpfsListDir(struct vnode *root, const char *pathname)
{
    if (checkPath(pathname) != 0)
        return -1;

    struct vnode **target;
    char component[NAME_BUFFER_SIZE];
    char *p, *cur;
    copynstr(pathname, component, NAME_BUFFER_SIZE); 
    p = component + 1;
    cur = component;
    
    target = (struct vnode **)allocDynamic(sizeof(struct vnode *));
    *target = root;

    uartPuts("tmpfs listdir: ");
    uartPuts(pathname);
    uartPuts("\n");

    while(1)
    {
        if (*p == '\0')
        {
            int32_t err = __vnodeLookup(target, cur);
            if (err != 0)
                return err;

            break;
        }

        if (*p == '/')
        {
            *p = '\0';

            int32_t err = __vnodeLookup(target, cur);
            if (err != 0)
                return err;

            cur = p + 1;
        }

        p++;
    }

    if ((*target)->type != dir)
        return -1;

    struct dentry *de = (struct dentry *)(*target)->internal;

    uartPuts("list nodes under ");
    uartPuts(pathname);
    uartPuts("\n");
    uartPuts("name    type\n");
    
    for (int32_t i = 0, end = de->child_num; i < end; ++i)
    {
        uartPuts(de->child[i]->name);
        uartPuts("   ");
        if (de->child[i]->type == file)
            uartPuts("file");
        else if (de->child[i]->type == dir)
            uartPuts("directory");
        uartPuts("\n");
    }

    return 0;
}

int32_t tmpfsFileWrite(struct file *file, const void *buf, size_t len)
{
    struct fcontent *file_content = (struct fcontent *)file->vnode->internal;
    char *cur_pos;
    size_t written;

    if ((file_content->fsize - file->f_pos) < len)
    {
        size_t new_fsize = round(file->f_pos + len);
        char *tmp = file_content->content;
        file_content->content = (char *)allocDynamic(new_fsize);
        copynstr(tmp, file_content->content, new_fsize);
        file_content->fsize = new_fsize;
        freeDynamic(tmp);
    }

    cur_pos = file_content->content + file->f_pos;
    written = copynstr((char *)buf, cur_pos, len);

    file->f_pos += written;

    return written;
}

int32_t tmpfsFileRead(struct file *file, void *buf, size_t len)
{
    struct fcontent *file_content = (struct fcontent *)file->vnode->internal;
    char *cur_pos = file_content->content + sizeof(char) * file->f_pos;
    size_t read = copynstr(cur_pos, (char *)buf, len);
    file->f_pos += read;

    return read;
}

int32_t tmpfsSetupMount(struct filesystem* fs, struct mount* mount)
{
    struct vnode *vn = (struct vnode *)allocSlot(fs->vnode_token);
    vn->name = (char *)allocSlot(fs->name_token);
    copynstr("/", vn->name, NAME_BUFFER_SIZE);
    vn->mount = mount;
    vn->v_ops = (struct vnodeOperations *)allocDynamic(sizeof(struct vnodeOperations));
    vn->v_ops->create = tmpfsVnodeCreate;
    vn->v_ops->lookup = tmpfsVnodeLookup;
    vn->v_ops->listdir = tmpfsListDir;
    vn->f_ops = (struct fileOperations *)allocDynamic(sizeof(struct fileOperations));
    vn->f_ops->write = tmpfsFileWrite;
    vn->f_ops->read = tmpfsFileRead;
    vn->type = dir;

    fs->dentry_token = getFreePool(sizeof(struct dentry));
    fs->fcontent_token = getFreePool(sizeof(struct fcontent));

    struct dentry *de = (struct dentry *)allocSlot(fs->dentry_token);
    de->vnode = vn;
    de->child_num = 0;
    de->child = (struct dentry **)allocDynamic(sizeof(struct dentry *) * MAX_DIR_CHILD);
    vn->internal = de;

    mount->fs = fs;
    mount->root = vn;

    return 0;
}
