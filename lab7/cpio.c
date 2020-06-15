#include <stdint.h>
#include "cpio.h"
#include "string_util.h"

int cpio_is_dir(uint64_t c_mode)
{
  if(c_mode & CPIO_DIR_BIT)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

void cpio_get_header(uint64_t header_base, struct cpio_header * header)
{
  /* no memcpy function, copy all attributes manually */
  /* Point of interset, other attributes are ignored */
  /* c_mode, c_filesize, c_namesize */
  char string_buffer[0x80];

  memcopy((char *)(header_base + CPIO_C_MODE_OFFSET), string_buffer, 8);
  string_buffer[8] = '\0';
  header -> c_mode = string_hex_char_to_longlong(string_buffer);
  memcopy((char *)(header_base + CPIO_C_FILESIZE_OFFSET), string_buffer, 8);
  string_buffer[8] = '\0';
  header -> c_filesize = string_hex_char_to_longlong(string_buffer);
  memcopy((char *)(header_base + CPIO_C_NAMESIZE_OFFSET), string_buffer, 8);
  string_buffer[8] = '\0';
  header -> c_namesize = string_hex_char_to_longlong(string_buffer);
  return;
}

uint64_t cpio_get_name_without_root(char * string_buff, const struct cpio_header * header, uint64_t header_base, uint64_t root_offset)
{
  uint64_t round_up_header_and_name = cpio_round_to_4x(header -> c_namesize + CPIO_HEADER_SIZE);

  /* return 0 if end of file, return the base of the file content otherwise */
  if(string_cmp((char *)(header_base + CPIO_HEADER_SIZE), "TRAILER!!!", 999))
  {
    return 0;
  }
  else
  {
    string_copy((char *)(header_base + CPIO_HEADER_SIZE + root_offset), string_buff);
    return header_base + round_up_header_and_name;
  }
}

uint64_t cpio_round_to_4x(uint64_t num)
{
  uint64_t residual = num % 4;
  if(residual == 0)
  {
    return num;
  }
  else
  {
    return num + (4 - residual);
  }
}

