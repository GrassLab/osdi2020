#include "framebuffer.h"

#include "img_data.h"
#include "mailbox.h"
#include "uart.h"

static FRAME_BUFFER fb;

void framebuffer_init ( )
{
    if ( !mbox_framebuffer_init ( FB_WIDTH, FB_HEIGHT, &fb ) )
    {
        uart_puts ( "Unable to set screen resolution to 1024x768x32\n" );
    }
}

void framebuffer_show_pic ( )
{
    unsigned int x, y;
    unsigned char * ptr = fb.fb_addr;
    char *data          = img_data, pixel[4];

    ptr += ( fb.height - img_height ) / 2 * fb.pitch + ( fb.width - img_width ) * 2;

    for ( y = 0; y < img_height; y++ )
    {
        for ( x = 0; x < img_width; x++ )
        {
            HEADER_PIXEL ( data, pixel );

            // the image is in RGB. So if we have an RGB framebuffer, we can copy the pixels
            // directly, but for BGR we must swap R (pixel[0]) and B (pixel[2]) channels.
            *( (unsigned int *) ptr ) = fb.isrgb ? *( (unsigned int *) &pixel ) : (unsigned int) ( pixel[0] << 16 | pixel[1] << 8 | pixel[2] );
            ptr += 4;
        }

        ptr += fb.pitch - img_width * 4;
    }
}