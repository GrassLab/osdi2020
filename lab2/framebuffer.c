#include "framebuffer.h"
#include "splash.h"

static uint8_t * framebuffer_base;
static uint32_t framebuffer_width;
static uint32_t framebuffer_height;
static uint32_t framebuffer_pitch;

void framebuffer_init(void)
{
  if(mailbox_framebuffer_init(800, 600, 24))
  {
    /* manual says some request must be checked, but i rentless ignore some of them */
    framebuffer_base = (uint8_t *)(uint64_t)(__mailbox_buffer[28] & 0x3FFFFFFF);
    framebuffer_width = __mailbox_buffer[5];
    framebuffer_height = __mailbox_buffer[6];
    framebuffer_pitch = __mailbox_buffer[33];
  }
  return;
}

void framebuffer_show_splash(void)
{
  uint8_t current_pixel[3];
  uint8_t * framebuffer_current = framebuffer_base;
  char * splash_ptr = splash_data;

  /* Centralize the splash to the middle of screen */
  /* x first */
  framebuffer_current += ((framebuffer_height - splash_height) / 2) * framebuffer_pitch;
  /* then y */
  framebuffer_current += (framebuffer_width - splash_width) / 2;

  /* row based iteration */
  for(unsigned y = 0; y < splash_height; ++y)
  {
    for(unsigned x = 0; x < splash_width; ++x)
    {
      HEADER_PIXEL(splash_ptr, current_pixel);
      for(unsigned i = 0; i < 3; ++i)
      {
        *framebuffer_current = current_pixel[i];
        ++framebuffer_current;
      }
    }
    /* to next line and exclude the width of splash */
    framebuffer_current += framebuffer_pitch - splash_width * 3;
  }
}

