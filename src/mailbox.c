#include "types.h"
#include "string.h"
#include "uart.h"
#include "mm.h"

#define GET_BOARD_REVISION  0x00010002
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

#define CHANNEL             8

typedef struct {
    uint8_t channel: 4;
    uint32_t data: 28;
} mail_message_t;

typedef struct {
    uint32_t size;
    uint32_t code;
    uint8_t tags[1];
} mail_buffer_t;

typedef struct {
    uint32_t id;
    uint32_t size;
    uint32_t code;
    uint8_t value;
    uint32_t padding: 16;
} mail_tag_t;

// copy paste from github
int mbox_call(unsigned int mbox[], unsigned char ch) {
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    /* wait until we can write to the mailbox */
    do{asm volatile("nop");}while(mm_read(MAILBOX_STATUS) & MAILBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    mm_write(MAILBOX_WRITE, r);
    uart_hex(r);
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{asm volatile("nop");}while(mm_read(MAILBOX_STATUS) & MAILBOX_EMPTY);
        /* is it a response to our message? */
        uart_hex(mm_read(MAILBOX_READ)); // 0x0000000F
        break;
        if(r == mm_read(MAILBOX_READ))
            /* is it a valid successful response? */
            return mbox[1]==REQUEST_SUCCEED;
    }
    return 0;
}

// my function, gives the same results as mbox_call
uint32_t mailbox_send(mail_message_t message, uint8_t channel) {
    while(mm_read(MAILBOX_STATUS) & MAILBOX_FULL);
    mm_write(MAILBOX_WRITE, &message);
    uart_hex(&message);
    while(true){
        while(mm_read(MAILBOX_STATUS) & MAILBOX_EMPTY);
        uint32_t data_r = mm_read(MAILBOX_READ);
        uint8_t channel = ((mail_message_t*)data_r)->channel;
        uint32_t data = (((mail_message_t*)data_r)->data << 4);
        mail_buffer_t *buffer = (mail_buffer_t*)data;
        mail_tag_t *tag = (mail_tag_t*)(buffer->tags[0]);
        uart_hex(data_r); // 0x0000000F
        break;
    }
}

void get_board_revision(){

    unsigned int mailbox[7] __attribute__ ((aligned (16)));

    mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[1] = REQUEST_CODE;
    // tags begin
    mailbox[2] = GET_BOARD_REVISION; // tag identifier
    mailbox[3] = 4; // maximum of request and response value buffer's length.
    mailbox[4] = TAG_REQUEST_CODE;
    mailbox[5] = 0; // value buffer
    // tags end
    mailbox[6] = END_TAG;


    mail_message_t msg;
    mail_buffer_t buffer;
    mail_tag_t tag;
    mail_buffer_t *buffer_p = &buffer;

    tag.id = (uint32_t)GET_BOARD_REVISION;
    tag.size = 4;
    tag.code = (uint32_t)TAG_REQUEST_CODE;
    tag.value = 0;
    tag.padding = 0;

    buffer_p->size = 7*4;
    buffer_p->code = REQUEST_CODE;
    memcpy(buffer_p->tags[0], (void *)&tag, sizeof(tag));
    buffer_p->tags[1] = END_TAG;

    msg.channel = (uint8_t)CHANNEL;
    msg.data = ((uint32_t)buffer_p >> 4);

    uart_puts("bruh\r\n");
    mailbox_send(msg, CHANNEL);
    uart_puts("\r\n");
    mbox_call(mailbox, CHANNEL);

    uart_puts("azaza\r\n");
}
