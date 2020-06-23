#ifndef FAT32
#define FAT32

struct directory_entry
{
  char name[8];			// 0x00
  char extension[3];		// 0x08
  char ignore1[9];		// 0x0b
  unsigned short start_hi;	// 0x14
  char ignore2[4];		// 0x16
  unsigned short start_lo;	// 0x1a
  unsigned int size;		// 0x1c
} __attribute__ ((packed));

struct boot_sector
{
  char ignore1[0xe];
  unsigned short count_of_reserved;	//0xe
  unsigned char num_of_fat;	// 0x10
  char ignore2[0x13];
  unsigned int sectors_per_fat;	// 0x24
  char ignore3[4];
  unsigned int cluster_num_of_root;	// 0x2c
} __attribute__ ((packed));

struct partition_entry
{
  char ignore[0x1c6];
  unsigned int lba;
  unsigned int size;
} __attribute__ ((packed));

struct fat32_info
{
  unsigned int lba;
  unsigned int size;
  unsigned short count_of_reserved;
  unsigned char num_of_fat;
  unsigned int sectors_per_fat;
  unsigned int cluster_num_of_root;
};

struct fat32_node
{
  struct fat32_info info;
  unsigned int cluster_index;
  unsigned int dir_entry;
  unsigned int dir_index;
};
#endif /* ifndef FAT32 */
