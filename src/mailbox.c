#include "mailbox.h"
#include "types.h"
#include "mini_uart.h"

uint32_t get_board_revision() {
  uint32_t __attribute__((aligned(16))) mailbox[7];

  // buffer size in bytes.
  mailbox[0] = sizeof(mailbox);
  // buffer request code ,0 means process request.
  mailbox[1] = 0;
  // Tag begin, 0x10002 is used to get board revision.
  mailbox[2] = 0x10002;
  // value buffer size in bytes.
  mailbox[3] = 4;
  // Request code.
  mailbox[4] = 0;
  // value buffer.
  mailbox[5] = 0;
  // Tag end.
  mailbox[6] = 0;

  mailbox_call(mailbox);
  return mailbox[5];
}

uint32_t get_vc_memory(void) {
  uint32_t __attribute__((aligned(16))) mailbox[8];

  // buffer size in bytes.
  mailbox[0] = sizeof(mailbox);
  // buffer request code ,0 means process request.
  mailbox[1] = 0;
  // Tag begin, 0x10006 is used to get board revision.
  mailbox[2] = 0x10006;
  // value buffer size in bytes.
  mailbox[3] = 8;
  // Request code.
  mailbox[4] = 0;
  // value buffer.
  mailbox[5] = 0;
  mailbox[6] = 0;
  // Tag end.
  mailbox[7] = 0;

  mailbox_call(mailbox);
  return mailbox[5];
}

void mailbox_call(uint32_t *mailbox) {
  uint32_t msg = ((uint32_t)mailbox & (~0xf)) | 0x8;
  while (*MAILBOX_STATUS & MAILBOX_FULL) {}
  *MAILBOX_WRITE = msg;
  while (true) {
    while (*MAILBOX_STATUS & MAILBOX_EMPTY) {}
    if (*MAILBOX_READ == msg) {
      return;
    }
  }
}
