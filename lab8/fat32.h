#ifndef __FAT32_H__
#define __FAT32_H__

#include "type.h"

#define SECTOR_SIZE (512)
#define FIRST_PARTITION_ENTRY (446) //0x1BE
#define isSUBDIRECTORY (16) //0x1000

#define CONCATE_8( a, b ) ( ( a ) | ( ( ( uint16_t ) ( b ) ) << 8 ) )

struct partition_entry {
	// 1 bytes
	uint8_t status;
	// 3 bytes
	uint8_t begin_head;
	uint16_t begin_sector;
	// 1 bytes
	uint8_t type;
	// 3 bytes
	uint8_t end_head;
	uint16_t end_sector;
	// 4 bytes
	uint32_t start_sector;
	// 4 bytes
	uint32_t number_of_sector;

	// other
	uint32_t root_sector;
	uint32_t cluster_start;
	uint32_t logic_sector_per_cluster;

}__attribute__ ( ( packed ) );

struct boot_sector {
    uint8_t jump[3];                   // 0-2
    uint8_t oem[8];                    // 3-10
    uint8_t bytes_per_logic_sector_0;  // 11
    uint8_t bytes_per_logic_sector_1;  // 12
    uint8_t logic_sector_per_cluster;  // 13
    uint16_t n_reserved_sectors;       // 14-15
    uint8_t n_file_alloc_tabs;         // 16
    uint8_t n_root_dir_entries_1;      // 17
    uint8_t n_root_dir_entries_2;      // 18
    uint16_t n_logical_sectors_16;     // 19-20
    uint8_t media;                     // 21
    uint16_t n_sector_per_fat_16;      // 22-23
    uint16_t phy_sector_per_track;     // 24-25
    uint16_t n_heads;                  // 26-27
    uint32_t n_hidden_sectors;         // 28-31
    uint32_t n_sectors_32;             // 32-35
    uint32_t n_sector_per_fat_32;      // 36-39
    uint16_t mirror_flag;              // 40-41
    uint16_t version;                  // 42-43
    uint32_t first_cluster;            // 44-47
    uint16_t info;                     // 48-49
    uint8_t other[17];                 // 50-66
    uint32_t s_number;                 // 67-70
    uint8_t label[11];                 // 71-81
    uint8_t type[8];                   // 82-89

    uint16_t bytes_per_logic_sector;  // 11-12
    uint16_t n_root_dir_entries;      // 17-18

} __attribute__ ( ( packed ) );

struct fat32_directory {
    char name[8];           // 0-7
    char ext[3];            // 8-10
    char attr[9];           // 11-19
    uint16_t cluster_high;  // 20-21
    uint32_t ext_attr;      // 22-25
    uint16_t cluster_low;   // 26-27
    uint32_t size;          // 28-31

} __attribute__ ( ( packed ) );




void init_fat32( void );

#endif