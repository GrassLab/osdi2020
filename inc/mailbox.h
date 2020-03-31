#ifndef _MAILBOX_H
#define _MAILBOX_H


/* a properly aligned buffer */
extern volatile unsigned int mbox[36];

#define MBOX_EMPTY      0x40000000 // Set if there is nothing to read from the mailbox
#define MBOX_FULL       0x80000000 // Set if there is no space to write into the mailbox

/**
 * Channels 
 */
#define MBOX_CH_MASK             0xF
#define MBOX_CH_POWER            0 // Power
#define MBOX_CH_FBUFF            1 // Framebuffer
#define MBOX_CH_VUART            2 // Virtual UART
#define MBOX_CH_VCHIQ            3 // VCHIQ
#define MBOX_CH_LEDS             4 // LEDs
#define MBOX_CH_BUTNS            5 // Buttons
#define MBOX_CH_TOUCH            6 // Touch Screen
#define MBOX_CH_COUNT            7 //
#define MBOX_CH_PROPT_ARM_VC     8 // Property tags ARM -> VC
#define MBOX_CH_PROPT_VC_ARM     9 // Property tags VC -> ARM

// /**
//  * Buffer request/response code
//  */
#define MBOX_REQUEST_CODE        0x00000000 // Process request
#define MBOX_RESPONSE_SUCCESS    0x80000000 // Request successful
#define MBOX_RESPONSE_FAILED     0x80000001 // Error parsing request buffer
#define MBOX_END_TAG             0x00000000

/**
 * Tags
 */
#define MBOX_TAG_REQUEST_CODE    0x00000000 
#define MBOX_TAG_RESPONSE_CODE   0x80000000 
#define MBOX_TAG_GET_BMODE       0x00010001 // Get board model
#define MBOX_TAG_GET_BREVI       0x00010002 // Get board revision
#define MBOX_TAG_GET_MAC         0x00010003 // Get board MAC address
#define MBOX_TAG_GET_BSERI       0x00010004 // Get board serial
#define MBOX_TAG_GET_ARMADDR     0x00010005 // Get ARM memory
#define MBOX_TAG_GET_VCADDR      0x00010006 // Get VC memory
#define MBOX_TAG_GET_CLK_STATE   0x00038001 // Get clock state
#define MBOX_TAG_SET_CLK_STATE   0x00038001 // Set clock state
#define MBOX_TAG_GET_CLK_RATE    0x00038002 // Get clock rate
#define MBOX_TAG_SET_CLK_RATE    0x00038002 // Set clock rate

/*
 * Unique clock IDs:
 */
// #define MBOX_CLK_ID_REVERSE 0x000000000: reserved
#define MBOX_CLK_ID_EMMC        0x000000001
#define MBOX_CLK_ID_UART        0x000000002
#define MBOX_CLK_ID_ARM         0x000000003
#define MBOX_CLK_ID_CORE        0x000000004
#define MBOX_CLK_ID_V3D         0x000000005
#define MBOX_CLK_ID_H264        0x000000006
#define MBOX_CLK_ID_ISP         0x000000007
#define MBOX_CLK_ID_SDRAM       0x000000008
#define MBOX_CLK_ID_PIXEL       0x000000009
#define MBOX_CLK_ID_PWM         0x00000000a
#define MBOX_CLK_ID_HEVC        0x00000000b
#define MBOX_CLK_ID_EMMC2       0x00000000c
#define MBOX_CLK_ID_M2MC        0x00000000d
#define MBOX_CLK_ID_PIXEL_BVB   0x00000000e


int mbox_call(unsigned char ch);
void get_board_revision();
void get_ARM_address();
void get_VC_address();

#endif//_MAILBOX_H