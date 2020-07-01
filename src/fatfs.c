#include "fatfs.h"

#include "io.h"
#include "pool.h"
#include "sdhost.h"
int fatfs_mount(struct filesystem* fs, struct mount* mount) {
    struct fat_BS* fat_boot = kmalloc(sizeof(struct fat_BS));
    readblock(2048, (char*)fat_boot);

    int fat_size = fat_boot->table_size_32;
    print_i(fat_size);
    print_s("\n");
    int root_dir_sectors = 0;
    print_i(root_dir_sectors);
    print_s("\n");
    int first_data_sector = fat_boot->reserved_sector_count +
                            (fat_boot->table_count * fat_size) +
                            root_dir_sectors;
    print_i(first_data_sector);
    print_s("\n");
    int first_fat_sector = fat_boot->reserved_sector_count;
    print_i(first_fat_sector);
    print_s("\n");
    int data_sectors = fat_boot->total_sectors_32 -
                       (fat_boot->reserved_sector_count +
                        (fat_boot->table_count * fat_size) + root_dir_sectors);
    print_i(data_sectors);
    print_s("\n");
    int total_clusters = data_sectors / fat_boot->sectors_per_cluster;
    print_i(total_clusters);
    print_s("\n");
    unsigned int root_cluster_32 = fat_boot->root_cluster;
    print_s("root_cluster_32: ");
    print_i(root_cluster_32);
    print_s("\n");
    int first_sector_of_cluster =
        ((root_cluster_32 - 2) * fat_boot->sectors_per_cluster) +
        first_data_sector;

    print_i(first_data_sector);
    print_s("\n");

    for (int i = 0; i < 8; i++) {
        print_c(fat_boot->oem_name[i]);
    }
    print_s("\n");
    print_i(first_sector_of_cluster);
    print_s("\n");
    char* fat_root = kmalloc(sizeof(char) * 512);
    readblock(first_sector_of_cluster + 2048, fat_root);
    asm volatile("clu:");
    print_s("\n");
}
n;