# Lab7 Questions

> Q: Is it possible that a file exists in a file system, but there is no vnode object of it?
A: Possible. If it is not opened or being point at. Then it resides in file system. vnode is created only when it is referenced.

> Q: Is EOF pass to the reader by a special character in the reader’s buffer?
A: No. When read system call detect the length requested is larger than the size recorded in file system. Function call will return the number of bytes actually read. There won't be character denoted as EOF. At least in raw system call.

> Q: Should each task owns its current working directory and the root directory?
A: Yes. Otherwise we cannot use relative pathname or chroot

