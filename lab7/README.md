# Lab 7 : Virtual File System

## Requirement 1

### File system registration
### Create a root file system
- [x] `required 1` Set up tmpfs as the root file system.

## Requirement 2

### vfs_open
### Component name lookup
- [x] `question 1` Is it possible that a file exists in a file system, but there is no vnode object of it?
in my case no, my vnode is bind by component and file

### Create a file
### Open a file
- [x] `required 2-1` implement `struct file* vfs_open(const char *pathname, int flags)`
- [x] `required 2-2` implement `int vfs_close(struct file* file)`

## Requirement 3

### vfs_write
- [x] `required 3-1` Implement `int vfs_write(struct file* file, const void* buf, size_t len)`
### vfs_read
- [x] `required 3-2` Implement `int vfs_read(struct file* file, void* buf, size_t len)`

- [x] `question 2` Is EOF pass to the reader by a special character in the reader’s buffer?
Yes. If we don't use '\0' to be EOF, when we read and write multiple time. It will cause error.

### Task’s VFS information

- [x] `question 3` Should each task owns its current working directory and the root directory?
Yes, sometime we need to access file by absolute addressing and relative addressing.
So we need both on each task.

