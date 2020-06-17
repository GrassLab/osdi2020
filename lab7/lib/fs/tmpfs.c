#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "mmu/kmalloc.h"
#include "utils.h"
#include "StringUtils.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct __Node {
    uint64_t is_dir;
    char *component_name;
    struct vnode **components;
    size_t capacity_of_component;
    size_t num_of_component;

    uint8_t *content;
    size_t file_size;
} Node;

static const size_t kDefaultComponentNum = 16;

static int lookup(struct vnode *dir_node, struct vnode **target, const char *pathname);
static int create(struct vnode *dir_node, struct vnode **target, const char *pathname);
static struct vnode_operations tmpfs_v_ops = { lookup, create };

static int lookup(struct vnode *dir_node, struct vnode **target, const char *pathname) {
    Node *internal_node = dir_node->internal;

    for (size_t i = 0; i < internal_node->num_of_component; ++i) {
        // FIXME: [workaround] assume there is only one dir "/"
        Node *component_internal = internal_node->components[i]->internal;
        if (compareString(component_internal->component_name, pathname) == 0) {
            *target = internal_node->components[i];
            return 0;
        }
    }
    return 1;
}

Node *newNode(const char *name, bool is_dir);

static int create(struct vnode *dir_node, struct vnode **target, const char *pathname) {
    struct vnode *new_vnode = kmalloc(sizeof(struct vnode));
    new_vnode->mount = dir_node->mount;
    new_vnode->v_ops = &tmpfs_v_ops;
    // TODO: f_ops
    new_vnode->internal = newNode(pathname, false);

    Node *internal_node = dir_node->internal;
    if (internal_node->num_of_component ==
        internal_node->capacity_of_component) {
        internal_node->capacity_of_component *= 2;

        struct vnode **new_components = kmalloc(
            sizeof(struct vnode *) * internal_node->capacity_of_component);
        memcpy(new_components, internal_node->components,
               sizeof(struct vnode *) * internal_node->num_of_component);
        kfree(internal_node->components);
        internal_node->components = new_components;
    }
    internal_node->components[internal_node->num_of_component++] = new_vnode;
    *target = new_vnode;
    
    return 0;
}

Node *newNode(const char *name, bool is_dir) {
    Node *node = kmalloc(sizeof(Node));
    size_t size = strlen(name);

    node->is_dir = is_dir;
    node->component_name = kmalloc(size + 1);
    memcpy(node->component_name, name, size + 1);

    // TODO: is_dir then allocate components, if not, set to NULL
    if (is_dir) {
        node->components = kmalloc(
            sizeof(struct vnode *) * kDefaultComponentNum);
        node->capacity_of_component = kDefaultComponentNum;
        node->num_of_component = 0;
        node->components[node->num_of_component] = NULL;
    }

    node->content = NULL;
    node->file_size = 0;

    return node;
}

void deleteNode(Node *node) {
    kfree(node->component_name);
    for (size_t i = 0; i < node->num_of_component; ++i) {
        deleteNode(node->components[i]->internal);
    }
    if (node->is_dir) {
        kfree(node->components);
    }
    kfree(node->content);
    kfree(node);
}

static int setupMount(struct filesystem *fs, struct mount *mount) {
    mount->root = kmalloc(sizeof(struct vnode));
    mount->root->mount = mount;
    mount->root->v_ops = &tmpfs_v_ops;
    // TODO: f_ops
    mount->root->internal = newNode("/", true);
    mount->fs = fs;

    // FIXME: meaningless return value
    return 0;
}

static void unsetMount(struct mount *mount) {
    // TODO: destruct internal of vnode
    deleteNode(mount->root->internal);
}

struct filesystem *newTmpFs(void) {
    struct filesystem *fs = kmalloc(sizeof(struct filesystem));
    fs->name = "tmpfs";
    fs->setup_mount = setupMount;
    fs->unset_mount = unsetMount;
    return fs;
}

// FIXME: haven't handle error code
int write(struct file *file, const void *buf, size_t len) {
    Node *internal_node = file->vnode->internal;

    if (internal_node->file_size <= file->f_pos + len) {
        // +1 for EOF
        uint8_t *new_content = kmalloc(file->f_pos + len + 1);
        memcpy(new_content, internal_node->content, internal_node->file_size);
        kfree(internal_node->content);

        internal_node->content = new_content;
        internal_node->file_size = file->f_pos + len + 1;
    }

    memcpy(internal_node->content + file->f_pos, buf, len);
    file->f_pos += len;

    return len;
}

// FIXME: haven't handle error code
int read(struct file *file, void *buf, size_t len) {
    Node *internal_node = file->vnode->internal;
    size_t min = (len > internal_node->file_size - file->f_pos - 1) ? internal_node->file_size - file->f_pos - 1 : len;

    memcpy(buf, internal_node->content + file->f_pos, min);
    file->f_pos += min;

    return min;
}

struct file_operations tmpfs_f_ops = { write, read };
