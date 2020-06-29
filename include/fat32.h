int fat32_register();
int fat32_setup_mount(struct filesystem* fs, struct mount* mount);

// vnode operations
int fat32_lookup(struct vnode* dir, struct vnode** target, const char* component_name);
int fat32_create(struct vnode* dir, struct vnode** target, const char* component_name);
int fat32_ls(struct vnode* dir);
int fat32_mkdir(struct vnode* dir, struct vnode** target, const char* component_name);

// file operations
int fat32_read(struct file* file, void* buf, uint64_t len);
int fat32_write(struct file* file, const void* buf, uint64_t len);