#include "gpio.h"
#include "tools.h"

/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

#define MBOX_REQUEST    0

/* channels */
#define MBOX_CH_POWER   0
#define MBOX_CH_FB      1
#define MBOX_CH_VUART   2
#define MBOX_CH_VCHIQ   3
#define MBOX_CH_LEDS    4
#define MBOX_CH_BTNS    5
#define MBOX_CH_TOUCH   6
#define MBOX_CH_COUNT   7
#define MBOX_CH_PROP    8

/* tags */
#define MBOX_TAG_LAST                       0
#define MBOX_TAG_GET_BOARD_REVISION         0x00010002
#define MBOX_TAG_GETSERIAL                  0x00010004
#define MBOX_TAG_GET_ARM                    0x00010005
#define MBOX_TAG_GET_VC                     0x00010006

int mbox_call(unsigned char ch);
void get_serial(void);
void get_board_revision(void);
void get_vc_information(void);
void get_arm_information(void);