#include "disk.h"
#include "uart0.h"
#include "typedef.h"

// error code: -1: inactive
static int mbr_partition_table_parse(struct mbr_partition* p, char* partition) {
    /*
     * https://en.wikipedia.org/wiki/Partition_type
     * 0b: FAT32 with CHS addressing
     */
    p->partition_type = *(partition + 4);
    if (p->partition_type == 0) return -1;

    p->first_sector_idx = *((int*)(partition + 8));
    p->sector_num = *((int*)(partition + 0xC));
    return 0;
}

void mbr_parse(struct disk* sd_card, char* sector) {
    // 0 - 445: Bootstrap

    // 446 - 461: Partition entry 1
    mbr_partition_table_parse(&sd_card->partition[0], sector + 446);

    // 462 - 477: Partition entry 2
    mbr_partition_table_parse(&sd_card->partition[1], sector + 462);

    // 478 - 493: Partition entry 3
    mbr_partition_table_parse(&sd_card->partition[2], sector + 478);

    // 494 - 509: Partition entry 4
    mbr_partition_table_parse(&sd_card->partition[3], sector + 494);

    // 510 - 511: Boot signature
}