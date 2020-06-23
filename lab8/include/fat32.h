#ifndef __FAT32_H__
#define __FAT32_H__

#include "fs.h"

extern struct filesystem *fat32;

void read_boot_sector();

struct __attribute__((__packed__)) mbr_partition {
     unsigned char     mbrp_flag;  /* MBR partition flags */
     unsigned char     mbrp_shd;   /* Starting head */
     unsigned char     mbrp_ssect; /* Starting sector */
     unsigned char     mbrp_scyl;  /* Starting cylinder */
     unsigned char     mbrp_type;  /* Partition type (see below) */
     unsigned char     mbrp_ehd;   /* End head */
     unsigned char     mbrp_esect; /* End sector */
     unsigned char     mbrp_ecyl;  /* End cylinder */
     unsigned int    mbrp_start; /* Absolute starting sector number */
     unsigned int    mbrp_size;  /* Partition size in sectors */
 }; /* __attribute__((__packed__)); */

struct __attribute__((__packed__))  mbr_bpbFAT32 {
  unsigned short    bpbBytesPerSec; /* bytes per sector */
  unsigned char     bpbSecPerClust; /* sectors per cluster */
  unsigned short    bpbResSectors;  /* number of reserved sectors */
  unsigned char     bpbFATs;    /* number of FATs */
  unsigned short    bpbRootDirEnts; /* number of root directory entries */
  unsigned short    bpbSectors; /* total number of sectors */
  unsigned char     bpbMedia;   /* media descriptor */
  unsigned short    bpbFATsecs; /* number of sectors per FAT */
  unsigned short    bpbSecPerTrack; /* sectors per track */
  unsigned short    bpbHeads;   /* number of heads */
  unsigned int    bpbHiddenSecs;  /* # of hidden sectors */
  unsigned int    bpbHugeSectors; /* # of sectors if bpbSectors == 0 */
  unsigned int    bpbBigFATsecs;  /* like bpbFATsecs for FAT32 */
  unsigned short    bpbExtFlags;    /* extended flags: */
#define MBR_FAT32_FATNUM    0x0F    /*   mask for numbering active FAT */
#define MBR_FAT32_FATMIRROR 0x80    /*   FAT is mirrored (as previously) */
  unsigned short    bpbFSVers;  /* filesystem version */
#define MBR_FAT32_FSVERS    0   /*   currently only 0 is understood */
  unsigned int    bpbRootClust;   /* start cluster for root directory */
  unsigned short    bpbFSInfo;  /* filesystem info structure sector */
  unsigned short    bpbBackup;  /* backup boot sector */
  unsigned char     bsReserved[12]; /* Reserved for future expansion */
  unsigned char     bsDrvNum;   /* Int 0x13 drive number (e.g. 0x80) */
  unsigned char     bsReserved1;    /* Reserved; set to 0 */
  unsigned char     bsBootSig;  /* 0x29 if next 3 fields are present */
  unsigned char     bsVolID[4]; /* Volume serial number */
  unsigned char     bsVolLab[11];   /* Volume label */
  unsigned char     bsFileSysType[8]; /* "FAT32   " */
}; /* __attribute__((__packed__)); */

#define MBR_PART_COUNT      4   /* Number of partitions in MBR */
#define MBR_BS_PARTNAMESIZE 8   /* Size of name mbr_bootsel nametab */

struct __attribute__((__packed__)) mbr_bootsel {
   unsigned char     mbrbs_defkey;
   unsigned char     mbrbs_flags;
   unsigned short    mbrbs_timeo;
   unsigned char     mbrbs_nametab[MBR_PART_COUNT][MBR_BS_PARTNAMESIZE + 1];
}; /* __attribute__((__packed__)); */

struct __attribute__((__packed__)) mbr_sector {
                   /* Jump instruction to boot code.  */
                   /* Usually 0xE9nnnn or 0xEBnn90 */
   unsigned char         mbr_jmpboot[3];
                   /* OEM name and version */
   unsigned char         mbr_oemname[8];
   struct mbr_bpbFAT32 mbr_bpb;
                   /* Boot code */
   unsigned char         mbr_bootcode[310];
                   /* Config for /usr/mdec/mbr_bootsel */
   struct mbr_bootsel  mbr_bootsel;
                   /* NT Drive Serial Number */
   unsigned int        mbr_dsn;
                   /* mbr_bootsel magic */
   unsigned short        mbr_bootsel_magic;
                   /* MBR partition table */
   struct mbr_partition    mbr_parts[MBR_PART_COUNT];
                   /* MBR magic (0xaa55) */
   unsigned short        mbr_magic;
}; /* __attribute__((__packed__)); */

/* http://www.c-jump.com/CIS24/Slides/FAT/F01_0180_sfn.htm */
struct __attribute__((__packed__)) SFN_entry {
  char filename[11];
  unsigned char attr;
  unsigned char reserved;
  unsigned char create_ms;
  unsigned short create_hms;
  unsigned short date;
  unsigned short access_date;
  unsigned short start_hi;
  unsigned short modify_time;
  unsigned short modify_date;
  unsigned short start_lo;
  unsigned int size;
};

#endif
