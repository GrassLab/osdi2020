#include "mailbox.h"

volatile uint32_t __attribute__((aligned(16))) __mailbox_buffer[0x40];
/*
 *  buffer (1 grid == uint8_t)
 * +------+    +------+    +------+    +------+    +------+    +------+
 * |Buffer|    |Buffer|    |Tag   |    |Tag   |    |Tag   |    |Tag   |
 * +size  +    +req/  +    +Id.   +    +buffer+    +req/  +    +------+
 * |in b  |    |resp  |    |      |    |size  |    |resp  |    |buffer|    Tag
 * +      + -> +code  + -> +      + -> +in b  + -> +code  + -> +------+ -> ...
 * |32b   |    |32b   |    |32b   |    |32b   |    |32b   |    |align |
 * +      +    +      +    +      +    +      +    +      +    +------+
 * |      |    |      |    |      |    |      |    |      |    |to 32b|
 * +------+    +------+    +------+    +------+    +------+    +------+
 *             +------+    +------+
 *             |End   |    |Pad   |
 *             +Tag   +    +------+
 *             |      |    |to    |
 *          -> +      + -> +------+
 *             |32b   |    |32b   |
 *             +      +    +------+
 *             |      |    |      |
 *             +------+    +------+
 */

/*
 * The buffer itself is 16-byte aligned as only the upper 28
 * bits of the address can be passed via the mailbox.
 * Put it as global or the stack will be smashed
 */

int mailbox_send_buffer(void)
{
  /* 1. Combine the message address (upper 28 bits) with channel number (lower 4 bits) */
  uint32_t ptr = (uint32_t)((uint64_t)__mailbox_buffer);
  ptr &= (uint32_t)(~0xF); /* Clear the LSB 4 bits */
  ptr |= (uint32_t)(MAILBOX_PROPERTY_ARM_TO_VC); /* Seto to channel number */

  /* 2. Check if Mailbox 0 status register's full flag is set. */
  while(*MAILBOX_STATUS & MAILBOX_FULL);

  /* 3. If not, then you can write to Mailbox 1 Read/Write register. */
  *MAILBOX_WRITE = ptr;

  /* 4. Check if Mailbox 0 status register's empty flag is set. */
  while(*MAILBOX_STATUS & MAILBOX_EMPTY);

  /* 5. If not, then you can read from Mailbox 0 Read/Write register. */
  /* 6. Check if the value is the same as you wrote in step 1. */
  if(ptr == *MAILBOX_READ && __mailbox_buffer[1] == MAILBOX_BUFFER_REQUEST_SUCCEED)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

uint32_t mailbox_get_board_revision(void)
{
  __mailbox_buffer[0] = MAILBOX_SINGLE_BUFFER_SIZE(MAILBOX_GET_BOARD_REVISION_REQ, MAILBOX_GET_BOARD_REVISION_RESP);
  __mailbox_buffer[1] = MAILBOX_BUFFER_REQUEST_CODE;
  __mailbox_buffer[2] = MAILBOX_GET_BOARD_REVISION;
  __mailbox_buffer[3] = MAX(MAILBOX_GET_BOARD_REVISION_REQ, MAILBOX_GET_BOARD_REVISION_RESP);
  __mailbox_buffer[4] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[5] = 0x0;
  __mailbox_buffer[6] = MAILBOX_TAG_END;

  if(mailbox_send_buffer())
  {
    return __mailbox_buffer[5];
  }
  else
  {
    return 0;
  }

}

int mailbox_get_vc_memory(void)
{

  __mailbox_buffer[0] = MAILBOX_SINGLE_BUFFER_SIZE(MAILBOX_GET_VC_MEMORY_REQ, MAILBOX_GET_VC_MEMORY_RESP);
  __mailbox_buffer[1] = MAILBOX_BUFFER_REQUEST_CODE;
  __mailbox_buffer[2] = MAILBOX_GET_VC_MEMORY;
  __mailbox_buffer[3] = MAX(MAILBOX_GET_VC_MEMORY_REQ, MAILBOX_GET_VC_MEMORY_RESP);
  __mailbox_buffer[4] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[5] = 0x0;
  __mailbox_buffer[6] = 0x0;
  __mailbox_buffer[7] = MAILBOX_TAG_END;

  if(mailbox_send_buffer())
  {
    return 1;
  }
  else
  {
    return 0;
  }

}

