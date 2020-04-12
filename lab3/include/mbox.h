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
#define GET_BOARD_REVISION   0x10002
#define GET_VC_MEMORY        0x10006
#define MBOX_TAG_SETCLKRATE  0x38002
#define MBOX_TAG_GETCLKRATE  0x30002
#define TAG_REQUEST_CODE           0 
#define MBOX_TAG_LAST              0

int mbox_call(unsigned char ch);
