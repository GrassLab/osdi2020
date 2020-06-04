#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#define FB_WIDTH  1024
#define FB_HEIGHT 768

struct framebuffer
{
    unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
    unsigned char * fb_addr;                  /* raw frame buffer address */
};

typedef struct framebuffer FRAME_BUFFER;

void framebuffer_init ( );
void framebuffer_show_pic ( );

#endif