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

int mailbox_get_arm_memory(void)
{

  __mailbox_buffer[0] = MAILBOX_SINGLE_BUFFER_SIZE(MAILBOX_GET_ARM_MEMORY_REQ, MAILBOX_GET_ARM_MEMORY_RESP);
  __mailbox_buffer[1] = MAILBOX_BUFFER_REQUEST_CODE;
  __mailbox_buffer[2] = MAILBOX_GET_ARM_MEMORY;
  __mailbox_buffer[3] = MAX(MAILBOX_GET_ARM_MEMORY_REQ, MAILBOX_GET_ARM_MEMORY_RESP);
  __mailbox_buffer[4] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[5] = 0x0;
  __mailbox_buffer[6] = 0x0;
  __mailbox_buffer[7] = MAILBOX_TAG_END;

  return mailbox_send_buffer();
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

  return mailbox_send_buffer();
}

int mailbox_set_clock_rate(uint32_t device_id, uint32_t clock_rate)
{
  __mailbox_buffer[0] = MAILBOX_SINGLE_BUFFER_SIZE(MAILBOX_SET_CLOCK_RATE_REQ, MAILBOX_SET_CLOCK_RATE_RESP);
  __mailbox_buffer[1] = MAILBOX_BUFFER_REQUEST_CODE;
  __mailbox_buffer[2] = MAILBOX_SET_CLOCK_RATE;
  __mailbox_buffer[3] = MAX(MAILBOX_SET_CLOCK_RATE_REQ, MAILBOX_SET_CLOCK_RATE_RESP);
  __mailbox_buffer[4] = MAILBOX_TAG_REQUEST_CODE; /* TODO: Diverge */
  __mailbox_buffer[5] = device_id;
  __mailbox_buffer[6] = clock_rate;
  __mailbox_buffer[7] = 0; /* Skip setting turbo */
  __mailbox_buffer[8] = MAILBOX_TAG_END;

  return mailbox_send_buffer();
}

int mailbox_framebuffer_init(uint32_t width, uint32_t height, uint32_t depth)
{
  /* Set then get according to manual */

  __mailbox_buffer[0]  = 35 * 4;
  __mailbox_buffer[1]  = MAILBOX_BUFFER_REQUEST_CODE;

  /* Set physical width, height */
  __mailbox_buffer[2]  = MAILBOX_SET_PHYSICAL_WH;
  __mailbox_buffer[3]  = MAX(MAILBOX_SET_PHYSICAL_WH_REQ, MAILBOX_SET_PHYSICAL_WH_RESP);
  __mailbox_buffer[4]  = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[5]  = width; /* width */
  __mailbox_buffer[6]  = height; /* height */

  /* Set virtual width, height */
  __mailbox_buffer[7]  = MAILBOX_SET_VIRTUAL_WH;
  __mailbox_buffer[8]  = MAX(MAILBOX_SET_VIRTUAL_WH_REQ, MAILBOX_SET_VIRTUAL_WH_RESP);
  __mailbox_buffer[9]  = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[10] = width; /* width */
  __mailbox_buffer[11] = height; /* height */

  /* Set virtual offset to 0, since physical == virtual */
  __mailbox_buffer[12] = MAILBOX_SET_VIRTUAL_OFFSET;
  __mailbox_buffer[13] = MAX(MAILBOX_SET_VIRTUAL_OFFSET_REQ, MAILBOX_SET_VIRTUAL_OFFSET_RESP);
  __mailbox_buffer[14] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[15] = 0; /* x offset is 0 */
  __mailbox_buffer[16] = 0; /* y offset is 0 */

  /* Set color bit depth */
  __mailbox_buffer[17] = MAILBOX_SET_DEPTH;
  __mailbox_buffer[18] = MAX(MAILBOX_SET_DEPTH_REQ, MAILBOX_SET_DEPTH_RESP);
  __mailbox_buffer[19] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[20] = depth; /* Full color and no alpha */ /* TODO: Diverge */

  /* Set pixel order */
  __mailbox_buffer[21] = MAILBOX_SET_PIXEL_ORDER;
  __mailbox_buffer[22] = MAX(MAILBOX_SET_PIXEL_ORDER_REQ, MAILBOX_SET_PIXEL_ORDER_RESP);
  __mailbox_buffer[23] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[24] = 1; /* RGB */

  /* Get framebuffer */
  __mailbox_buffer[25] = MAILBOX_GET_FRAMEBUFFER;
  __mailbox_buffer[26] = MAX(MAILBOX_GET_FRAMEBUFFER_REQ, MAILBOX_GET_FRAMEBUFFER_RESP);
  __mailbox_buffer[27] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[28] = width * (depth / 8); /* buffer_width * bytes_per_pixel */ /* will return pointer to framebuffer */
  __mailbox_buffer[29] = 0; /* will return size of framebuffer */

  /* Get pitch */
  __mailbox_buffer[30] = MAILBOX_GET_PITCH;
  __mailbox_buffer[31] = MAX(MAILBOX_GET_PITCH_REQ, MAILBOX_GET_PITCH_RESP);
  __mailbox_buffer[32] = MAILBOX_TAG_REQUEST_CODE;
  __mailbox_buffer[33] = 0; /* will get pitch */

  /* End of mailbox */
  __mailbox_buffer[34] = MAILBOX_TAG_END;

  return mailbox_send_buffer();
}

