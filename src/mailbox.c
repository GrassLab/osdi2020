#include "mailbox.h"

#include "framebuffer.h"
#include "type.h"

/** https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes#general-procedure */
/** retun 0 on failuer, non-zerp on success */
int mailbox_call ( unsigned char channel, volatile uint32_t * mail_box )
{
    const uint32_t interface = ( (unsigned int) ( (unsigned long) mail_box ) & ~0xF ) | ( channel & 0xF );

    /* wait until  the full flag is not set */
    do
    {
        asm volatile( "nop" );

    } while ( *MAILBOX_REG_STATUS & MAILBOX_FULL );

    /* write the address of our message to the mailbox with channel identifier */
    *MAILBOX_REG_WRITE = interface;

    while ( 1 )
    {
        /* check if the response is exist */
        do
        {
            asm volatile( "nop" );

        } while ( *MAILBOX_REG_STATUS & MAILBOX_EMPTY );

        /** check is our message or not */
        if ( *MAILBOX_REG_READ == interface )
        {
            /* is it a valid successful response? */
            return mail_box[1] == TAGS_REQ_SUCCEED;
        }
    }

    return 0;
}

/** https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface */
uint32_t mbox_get_board_revision ( )
{
    volatile uint32_t __attribute__ ( ( aligned ( 16 ) ) ) mail_box[36];

    mail_box[0] = 7 * 4;  // buffer size in bytes
    mail_box[1] = TAGS_REQ_CODE;

    // tags begin
    mail_box[2] = TAGS_HARDWARE_BOARD_REVISION;  // tag identifier
    mail_box[3] = 4;                             // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;
    mail_box[5] = 0;  // value buffer
    // tags end
    mail_box[6] = TAGS_END;

    if ( mailbox_call ( MAILBOX_CH_PROP, mail_box ) )
    {
        return mail_box[5];
    }
    else
    {
        return 0;
    }
}

uint64_t mbox_get_VC_base_addr ( )
{
    volatile uint32_t __attribute__ ( ( aligned ( 16 ) ) ) mail_box[36];

    mail_box[0] = 8 * 4;  // buffer size in bytes
    mail_box[1] = TAGS_REQ_CODE;

    // tags begin
    mail_box[2] = TAGS_HARDWARE_VC_MEM;  // tag identifier
    mail_box[3] = 8;                     // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;
    mail_box[5] = 0;  // value buffer
    mail_box[6] = 0;  // value buffer
    // tags end
    mail_box[7] = TAGS_END;

    if ( mailbox_call ( MAILBOX_CH_PROP, mail_box ) )
    {
        return ( ( uint64_t ) ( mail_box[5] ) ) << 32 | mail_box[6];
    }
    else
    {
        return 0;
    }
}

void mbox_set_clock_to_PL011 ( )
{
    volatile uint32_t __attribute__ ( ( aligned ( 16 ) ) ) mail_box[36];

    mail_box[0] = 9 * 4;
    mail_box[1] = TAGS_REQ_CODE;

    // tags begin
    mail_box[2] = TAGS_SET_CLOCK;  // set clock rate
    mail_box[3] = 12;              // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;
    mail_box[5] = CLOCK_ID_UART;  // clock id: UART clock
    mail_box[6] = 4000000;        // rate: 4Mhz
    mail_box[7] = 0;              // clear turbo
    mail_box[8] = TAGS_END;

    mailbox_call ( MAILBOX_CH_PROP, mail_box );
}

int mbox_framebuffer_init ( uint32_t width, uint32_t height, FRAME_BUFFER * fb )
{
    volatile uint32_t __attribute__ ( ( aligned ( 16 ) ) ) mail_box[36];

    mail_box[0] = 35 * 4;
    mail_box[1] = TAGS_REQ_CODE;

    mail_box[2] = FB_PHY_WID_HEIGHT_GET;  // set physical width and height
    mail_box[3] = 8;                      // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;
    mail_box[5] = width;   // width
    mail_box[6] = height;  // height

    mail_box[7]  = FB_VIR_WID_HEIGHT_SET;  // set virtual width and height
    mail_box[8]  = 8;                      // maximum of request and response value buffer's length.
    mail_box[9]  = TAGS_REQ_CODE;
    mail_box[10] = width;   // virtual width
    mail_box[11] = height;  // virtual height

    mail_box[12] = FB_VIR_OFFSET_SET;  // set virtual offset
    mail_box[13] = 8;                  // maximum of request and response value buffer's length.
    mail_box[14] = TAGS_REQ_CODE;
    mail_box[15] = 0;  // x offset
    mail_box[16] = 0;  // y offset

    mail_box[17] = FB_DEPTH_SET;  // set depth
    mail_box[18] = 4;             // maximum of request and response value buffer's length.
    mail_box[19] = TAGS_REQ_CODE;
    mail_box[20] = 32;  // depth of 32 bits

    mail_box[21] = FB_PIXEL_ORDER_SET;  // set pixel order
    mail_box[22] = 4;                   // maximum of request and response value buffer's length.
    mail_box[23] = TAGS_REQ_CODE;
    mail_box[24] = 1;  // RGB, not BGR preferably

    mail_box[25] = FB_ALLOC_BUFFER;  // get framebuffer, gets alignment on request
    mail_box[26] = 8;                // maximum of request and response value buffer's length.
    mail_box[27] = TAGS_REQ_CODE;
    mail_box[28] = 4096;  // framebuffer pointer
    mail_box[29] = 0;     // framebuffer size

    mail_box[30] = FB_PITCH_GET;  // get pitch
    mail_box[31] = 4;             // maximum of request and response value buffer's length.
    mail_box[32] = TAGS_REQ_CODE;
    mail_box[33] = 0;  // pitch

    mail_box[34] = TAGS_END;

    // this might not return exactly what we asked for, could be
    // the closest supported resolution instead
    if ( mailbox_call ( MAILBOX_CH_PROP, mail_box ) && mail_box[20] == 32 && mail_box[28] != 0 )
    {
        mail_box[28] &= 0x3FFFFFFF;  // convert GPU address to ARM address
        fb->width   = mail_box[5];   // get actual physical width
        fb->height  = mail_box[6];   // get actual physical height
        fb->pitch   = mail_box[33];  // get number of bytes per line
        fb->isrgb   = mail_box[24];  // get the actual channel order
        fb->fb_addr = (void *) ( (unsigned long) mail_box[28] );

        return 1;
    }
    else
    {
        return 0;
    }
}