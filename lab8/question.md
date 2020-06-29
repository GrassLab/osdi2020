# Lab8 Questions

> Q: Explain how an OS supports automatically mounting file systems after plug in a USB flash drive into a computer. 
A:
  - Get device info from device tree
  - Determine the device is a block device
  - Use `mknod` to create a file for the device
  - Use USB controller driver to read/write the device by the file
  - Read the MBR/GPT table on the device to find the partitions and their filesystem type
  - Use the corresponding filesystem driver and create devices for each partition

> Q: How to implement a component cache mechanism if a file has multiple names (because of hard links)

A: In FAT32, use start of cluster as index, with multiple component name. 

> Q: Does tmpfs need sync method?
A: No. All of the contents exist on RAM. It does not even need any cache mechanism.

