#include "loadimg.h"
#include "string_util.h"
#include "sumhash.h"

int loadimg(uint32_t offset, int init_uart)
{
  char uart_buffer[0x104];
  uint32_t filesize;
  uint32_t total_0x102_byte_blocks;
  uint32_t residual_block_bytes = 0;
  uint16_t hash;

  if(init_uart != 0)
  {
    uart_init();
  }

  /* wait for "\x00H3lloloadimg\x00" */
  while(1)
  {
    if(uart_getc(0) == '\n')
    {
      uart_puts("H3llosendimg");
    }
    uart_getn(uart_buffer, 13);
    if(string_cmp(uart_buffer, "H3lloloadimg", 13) != 0)
    {
      break;
    }
  }

  /* wait for 4 byte little endian size */
  uart_getn(uart_buffer, 4);
  filesize = *(uint32_t *)uart_buffer;

  total_0x102_byte_blocks = (uint32_t)(filesize / 0x100);
  if(((filesize % 0x100) + 2) % 100 == 0)
  {
    ++total_0x102_byte_blocks;
  }
  else
  {
    residual_block_bytes = (filesize % 0x100);
  }

  for(uint32_t current_block = 0; current_block < total_0x102_byte_blocks; ++current_block)
  {
    uart_getn((char *)(uint64_t)(offset + current_block * 0x100), 0x100);
    uart_getn((char *)&hash, 0x2);
    if(!sumhash_check((uint8_t *)(uint64_t)(offset + current_block * 0x100), hash, 0x100))
    {
      uart_puts("DE");
      /* corrupted */
      return 0;
    }
    else if(residual_block_bytes == 0 && current_block == total_0x102_byte_blocks - 1)
    {
      uart_puts("BY");
      return 1;
    }
    else
    {
      uart_puts("FI");
    }
  }
  uart_getn((char *)(uint64_t)offset + (total_0x102_byte_blocks) * 0x100, residual_block_bytes);
  uart_getn((char *)&hash, 2);
  if(!sumhash_check((uint8_t *)(uint64_t)(offset + (total_0x102_byte_blocks) * 0x100), hash, residual_block_bytes))
  {
    uart_puts("DE");
    return 0;
  }
  else
  {
    uart_puts("BY");
    return 1;
  }
}

