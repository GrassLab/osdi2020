#include "mailbox.h"
#include "type.h"

/** https://github.com/raspberrypi/firmware/wiki/Accessing-mailboxes#general-procedure */
/** retun 0 on failuer, non-zerp on success */
int mailbox_call ( unsigned char channel, volatile unsigned int * mail_box )
{   
    const uint32_t interface = ((unsigned int)((unsigned long)mail_box)&~0xF) | (channel & 0xF);

    /* wait until  the full flag is not set */
    do
    {
        asm volatile("nop");

    } while ( *MAILBOX_REG_STATUS & MAILBOX_FULL );

    /* write the address of our message to the mailbox with channel identifier */
    *MAILBOX_REG_WRITE = interface;

    while(1)
    {
        /* check if the response is exist */
        do
        {
            asm volatile("nop");
        
        } while( *MAILBOX_REG_STATUS & MAILBOX_EMPTY );

        /** check is our message or not */
        if( *MAILBOX_REG_READ == interface )
        {
            /* is it a valid successful response? */
            return mail_box[1] == TAGS_REQ_SUCCEED;
        }
    }

    return 0;
}

/** https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface */
uint32_t mbox_get_board_revision ()
{
    volatile unsigned int  __attribute__((aligned(16))) mail_box[36];

    mail_box[0] = 7 * 4;                 // buffer size in bytes
    mail_box[1] = TAGS_REQ_CODE;
    
    // tags begin
    mail_box[2] = TAGS_BOARD_REVISION;   // tag identifier
    mail_box[3] = 4;                     // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;
    mail_box[5] = 0;                     // value buffer
    // tags end
    mail_box[6] = TAGS_END;

    if ( mailbox_call( MAILBOX_CH_PROP, mail_box ) )
    {
        return mail_box[5];
    }
    else
    {
        return 0;
    }
}

uint64_t mbox_get_VC_base_addr ()
{
    volatile unsigned int  __attribute__((aligned(16))) mail_box[36];

    mail_box[0] = 8 * 4;                 // buffer size in bytes
    mail_box[1] = TAGS_REQ_CODE;
    
    // tags begin
    mail_box[2] = TAGS_VC_MEM;           // tag identifier
    mail_box[3] = 8;                     // maximum of request and response value buffer's length.
    mail_box[4] = TAGS_REQ_CODE;         // operation
    mail_box[5] = 0;                     // value buffer
    mail_box[6] = 0;                     // value buffer
    // tags end
    mail_box[7] = TAGS_END;

    if ( mailbox_call( MAILBOX_CH_PROP, mail_box ) )
    {
        return ((uint64_t)(mail_box[5])) << 32 | mail_box[6];
    }
    else
    {
        return 0;
    }
}