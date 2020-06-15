# Lab 7 : Virtual File System

## Requirement 1

### File system registration
### Create a root file system
- [x] `required 1` Set up tmpfs as the root file system.

## Requirement 2

### vfs_open
### Component name lookup
- [ ] `question 1` Is it possible that a file exists in a file system, but there is no vnode object of it?
in my case no, my vnode is bind by component and file

### Create a file
### Open a file
- [ ] `required 2-1` implement `struct file* vfs_open(const char *pathname, int flags)`
- [ ] `required 2-2` implement `int vfs_close(struct file* file)`

## Requirement 3

### vfs_write
- [ ] `required 3-1` Implement `int vfs_write(struct file* file, const void* buf, size_t len)`
### vfs_read
- [ ] `required 3-2` Implement `int vfs_read(struct file* file, void* buf, size_t len)`

- [ ] `question 2` Is EOF pass to the reader by a special character in the reader’s buffer?

### Task’s VFS information

- [ ] `question 3` Should each task owns its current working directory and the root directory?


