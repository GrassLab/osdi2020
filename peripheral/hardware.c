#include <string.h>
#include "mbox.h"
#include "hardware.h"

unsigned int
hardware_info_revision ()
{
  struct mbox_msg *msg;
  msg = (struct mbox_msg *) &mbox;

  // set tag
  msg->tag.id = 0x00010002;
  msg->tag.buf_size = 4;
  msg->tag.code = 0;
  // set message
  msg->buf_size = sizeof (struct mbox_msg) + msg->tag.buf_size + 4;
  msg->code = 0;
  // set tag buf and tag end
  bzero (msg + 1, msg->tag.buf_size + 4);
  mbox_call (MBOX_CH_PROP);

  return *(unsigned int *) (msg + 1);
}

unsigned int
hardware_info_VC_base ()
{
  struct mbox_msg *msg;
  msg = (struct mbox_msg *) &mbox;

  // set tag
  msg->tag.id = 0x00010006;
  msg->tag.buf_size = 8;
  msg->tag.code = 0;
  // set mseeage
  msg->buf_size = sizeof (struct mbox_msg) + msg->tag.buf_size + 4;
  msg->code = 0;
  // set tag buf and tag end
  bzero (msg + 1, msg->tag.buf_size + 4);
  mbox_call (MBOX_CH_PROP);

  return *(unsigned int *) (msg + 1);
}

size_t
hardware_info_memory_size ()
{
  unsigned int *base_addr;
  unsigned int *size;
  struct mbox_msg *msg;
  msg = (struct mbox_msg *) &mbox;
  // set tag
  msg->tag.id = 0x00010005;
  msg->tag.buf_size = 8;
  msg->tag.code = 0;
  // set mseeage
  msg->buf_size = sizeof (struct mbox_msg) + msg->tag.buf_size + 4;
  msg->code = 0;
  // set tag buf and tag end
  bzero (msg + 1, msg->tag.buf_size + 4);
  mbox_call (MBOX_CH_PROP);

  base_addr = (unsigned int *) (msg + 1);
  size = base_addr + 1;
  // return *size;
  return 1024 * 1024 * 100;
}
