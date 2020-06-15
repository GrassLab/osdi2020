#include <stdint.h>
#include "ramfs.h"
#include "cpio.h"
#include "uart.h"
#include "sys.h"
#include "vfs.h"
#include "string_util.h"

void ramfs_init(void)
{
  char string_buffer[0x80];
  uint64_t header_base = (uint64_t)&_binary_initramfs_cpio_start;
  struct cpio_header header;

  /* get the header of root directory */
  cpio_get_header(header_base, &header);

  /* calculate the offset of the first wanted file/dir */
  header_base += cpio_round_to_4x(CPIO_HEADER_SIZE + header.c_namesize);

  /* obtain every file/dir */
  while(1)
  {
    cpio_get_header(header_base, &header);
    uint64_t file_content_start = cpio_get_name_without_root(string_buffer, &header, header_base, RAMFS_INITRAMFS_LENGTH);
    if(file_content_start == 0)
    {
      /* get TRAILER!!! */
      break;
    }

    /* pre scheduling/there is no relative directory */
    if(cpio_is_dir(header.c_mode))
    {
      sys_mkdir(string_buffer);
    }
    else
    {
      vfs_write(vfs_open(string_buffer, O_CREAT), (char *)file_content_start, header.c_filesize);
      //memcopy((char *)file_content_start, string_buffer, (unsigned)header.c_filesize);
    }

    /* calculate offset of next header */
    header_base += cpio_round_to_4x(CPIO_HEADER_SIZE + header.c_namesize);
    if(!cpio_is_dir(header.c_mode))
    {
      header_base += cpio_round_to_4x(header.c_filesize);
    }
  }

  uart_puts("ramfs init complete\n");
  return;
}

