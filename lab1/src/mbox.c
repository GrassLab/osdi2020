/* https://github.com/raspberrypi/firmware/wiki/Mailbox-property-interface */

#include "io.h"
#include "mbox.h"
#include "util.h"

int mbox_call(unsigned int* mbox, unsigned char chan){
    unsigned int r = (((unsigned int)((unsigned long)mbox)&~0xF) | (chan&0xF));
    while(get32(MBOX_STATUS) & MBOX_FULL) __asm__ volatile("nop");
    put32(MBOX_WRITE, r);
    while(1) {
        while(get32(MBOX_STATUS) & MBOX_EMPTY) __asm__ volatile("nop");
        if(r == get32(MBOX_READ)) /* is it valid? */
            return mbox[1]==REQUEST_SUCCEED;
    }
    puts("here");
    return 0;
}

void get_board_revision(){
  unsigned int mbox[7];
  mbox[0] = 7 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_BOARD_REVISION; // tag identifier
  mbox[3] = 4; // maximum of request and response value buffer's length.
  mbox[4] = TAG_REQUEST_CODE;
  mbox[5] = 0; // value buffer
  // tags end
  mbox[6] = END_TAG;

  if(mbox_call(mbox, MBOX_CH_PROP)) printf("0x%x\n", mbox[5]); // it should be 0xa020d3 for rpi3 b+
  else puts("get_board_reversion() failed");
}

void get_vc_memaddr(){
  unsigned int mbox[8];
  mbox[0] = 8 * 4; // buffer size in bytes
  mbox[1] = REQUEST_CODE;
  // tags begin
  mbox[2] = GET_VC_MEMORY; // tag identifier
  mbox[3] = 4; // maximum of request and response value buffer's length.
  mbox[4] = TAG_REQUEST_CODE;
  mbox[5] = 0; // address
  mbox[6] = 0; // size
  // tags end
  mbox[7] = END_TAG;

  if(mbox_call(mbox, MBOX_CH_PROP)) printf("0x%x\n", mbox[5]); // it should be 0xa020d3 for rpi3 b+
  else puts("get_vc_memaddr() failed");
}
