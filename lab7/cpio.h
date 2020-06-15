#ifndef __CPIO_H__
#define __CPIO_H__

#define CPIO_C_MODE_OFFSET 14u
#define CPIO_C_FILESIZE_OFFSET 54u
#define CPIO_C_NAMESIZE_OFFSET 94u
#define CPIO_HEADER_SIZE 110u
#define CPIO_DIR_BIT 0040000

/*
struct ramfs_cpio_newc_header
{
  // no pack issue as they are all char
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
 };
*/

struct cpio_header
{
  uint64_t c_mode;
  uint64_t c_filesize;
  uint64_t c_namesize;
};

int cpio_is_dir(uint64_t c_mode);
void cpio_get_header(uint64_t header_base, struct cpio_header * header);
uint64_t cpio_get_name_without_root(char * string_buff, const struct cpio_header * header, uint64_t header_base, uint64_t root_offset);
uint64_t cpio_round_to_4x(uint64_t num);

#endif

