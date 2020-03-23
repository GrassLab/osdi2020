#include "MiniUart.h"
#include "peripheral/base.h"

/* ==============================
 * > Mailbox
 * ============================== */
#define MAILBOX_SIZE 7u

// only upper 28 bits of message address can be passed
// lower 4 bits will be truncated, so it must be 16-byte aligned (4-bit: 0~15).
static const unsigned int kAlignmentSizeOfMailbox = 16u;
static const unsigned int kRequestCode = 0x00000000u;
static const unsigned int kGetBoardRevision = 0x00010002;
static const unsigned int kTagRequestCode = 0x00000000u;
static const unsigned int kEndTag = 0x00000000u;

static const unsigned int kChannelNum = 8u;

// registers
#define MAILBOX_BASE MMIO_BASE + 0xB880ll
static volatile unsigned int *const pMailboxRead =
    (unsigned int *)(MAILBOX_BASE + 0x0);
static volatile unsigned int *const pMailboxStatus =
    (unsigned int *)(MAILBOX_BASE + 0x18);
static volatile unsigned int *const pMailboxWrite =
    (unsigned int *)(MAILBOX_BASE + 0x20);
static const unsigned int kMailboxResponse = 0x80000000;
static const unsigned int kMailboxFull = 0x80000000;
static const unsigned int kMailboxEmpty = 0x40000000;

// return 0 on failed response, 1 on success
static int sendMessage(volatile unsigned int *msg) {
    // combine the address of msg with channel identifier
    unsigned int addr =
        (unsigned int)(((unsigned long)msg & ~0xF) | (kChannelNum & 0xF));

    // wait until mailbox is writable
    while (*pMailboxStatus & kMailboxFull)
        ;

    *pMailboxWrite = addr;

    // wait until response
    while (*pMailboxStatus & kMailboxEmpty)
        ;

    // chech whether this is a response to us
    if (addr == *pMailboxRead) {
        return msg[1] == kMailboxResponse;
    }

    return 0;
}

unsigned int getBoardRevision(void) {
    volatile unsigned int __attribute__((aligned(16u)))
    mailbox[MAILBOX_SIZE] = {
        MAILBOX_SIZE * sizeof(unsigned int), // buffer size in bytes
        kRequestCode,

        /* tags structure
         *   0: tag identifier
         *   1: value buffer size in bytes
         *   2: zero
         *   3~: optional value buffer
         */
        // tags begin
        kGetBoardRevision, (MAILBOX_SIZE - (3u + 3u)), kTagRequestCode, 0,
        // tags end

        kEndTag};

    if (!sendMessage(mailbox)) {
        sendStringUART("[LOG][ERROR] fail to send message\n");
    }

    return mailbox[5];
}
