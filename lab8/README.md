# Lab 8 : File System Meets Hardware

## Requirement 1

### Get the FAT32 partition
- [x] `required 1-1` Get the FAT32 partition.

### Set up a FAT32 mount
- [x] `required 1-2` Parse the FAT32’s metadata and set up the mount.

## Requirement 2

### Lookup and open a file in FAT32
- [x] `required 2-1 ` Look up and open a file in FAT32.

### Read/Write a file in FAT32
- [x] `required 2-2` Read/Write a file in FAT32.

- [x] `question 1` Explain how an OS supports automatically mounting file systems after plug in a USB flash drive into a computer.
https://www.kernel.org/doc/html/v4.13/driver-api/usb/hotplug.html

1. Find a driver that can handle the device. That may involve loading a kernel module; newer drivers can use module-init-tools to publish their device (and class) support to user utilities.
2. Bind a driver to that device. Bus frameworks do that using a device driver’s probe() routine.
3. Tell other subsystems to configure the new device. Print queues may need to be enabled, networks brought up, disk partitions mounted, and so on. In some cases these will be driver-specific actions.

- [x] `question 2` How to implement a component cache mechanism if a file has multiple names (because of hard links)

use linked list to store same file but different name

struct name_node{
    char * name;
    node * next;
}

A -> null;

A -> B -> null;

- [x] `question 3 `Does tmpfs need sync method?

存放暫存檔案的tmpfs, 沒有external storage, 所以不用sync method

