#include "homer.h"
#include "mbox.h"
#include "uart.h"
#define P_WIDTH 512
#define P_HEIGHT 512

unsigned int width, height, pitch, isrgb; /* dimensions and channel order */
unsigned char *lfb;						  /* raw frame buffer address */

/**
 * Set screen resolution to 1024x768
 */
void lfb_init()
{
	mbox[0] = 35 * 4;
	mbox[1] = MBOX_REQUEST;
	/*
  Set physical (display) width/height
  Tag: 0x00048003
  Request:
  Length: 8
  Value:
  u32: width in pixels
  u32: height in pixels
  Response:
  Length: 8
  Value:
  u32: width in pixels
  u32: height in pixels
  */
	mbox[2] = MBOX_TAG_SET_DISPLAY_WH; // set phy wh
	mbox[3] = 8;
	mbox[4] = MBOX_TAG_REQUEST_CODE;
	mbox[5] = P_WIDTH;	// FrameBufferInfo.width
	mbox[6] = P_HEIGHT; // FrameBufferInfo.height

	/*
  Set virtual (buffer) width/height
  Tag: 0x00048004
  Request:
  Length: 8
  Value:
  u32: width in pixels
  u32: height in pixels
  Response:
  Length: 8
  Value:
  u32: width in pixels
  u32: height in pixels
  */
	mbox[7] = MBOX_TAG_SET_VIRTUAL_WH; // set virt wh
	mbox[8] = 8;
	mbox[9] = MBOX_TAG_REQUEST_CODE;
	mbox[10] = P_WIDTH;	 // FrameBufferInfo.virtual_width
	mbox[11] = P_HEIGHT; // FrameBufferInfo.virtual_height

	/*
  Set virtual offset
  Tag: 0x00048009
  Request:
  Length: 8
  Value:
  u32: X in pixels
  u32: Y in pixels
  Response:
  Length: 8
  Value:
  u32: X in pixels
  u32: Y in pixels
  */
	mbox[12] = MBOX_TAG_SET_VIRTUAL_OFFSET; // set virt offset
	mbox[13] = 8;
	mbox[14] = MBOX_TAG_REQUEST_CODE;
	mbox[15] = 0; // FrameBufferInfo.x_offset
	mbox[16] = 0; // FrameBufferInfo.y.offset

	/*
  Set depth
  Tag: 0x00048005
  Request:
  Length: 4
  Value:
  u32: bits per pixel
  Response:
  Length: 4
  Value:
  u32: bits per pixel
  */
	mbox[17] = MBOX_TAG_SET_DEPTH; // set depth
	mbox[18] = 4;
	mbox[19] = MBOX_TAG_REQUEST_CODE;
	mbox[20] = 32; // FrameBufferInfo.depth

	/*
  Set pixel order
  Tag: 0x00048006
  Request:
  Length: 4
  Value:
  u32: state (as above)
  Response:
  Length: 4
  Value:
  u32: state (as above)
  */
	mbox[21] = MBOX_TAG_SET_PIXEL_ORDER; // set pixel order
	mbox[22] = 4;
	mbox[23] = MBOX_TAG_REQUEST_CODE;
	mbox[24] = 1; // RGB, not BGR preferably

	/*
  Allocate buffer
  Tag: 0x00040001
  Request:
  Length: 4
  Value:
  u32: alignment in bytes
  Response:
  Length: 8
  Value:
  u32: frame buffer base address in bytes
  u32: frame buffer size in bytes
  */
	mbox[25] =
		MBOX_TAG_ALLOCATE_BUF; // get framebuffer, gets alignment on request
	mbox[26] = 8;
	mbox[27] = MBOX_TAG_REQUEST_CODE;
	mbox[28] = 4096; // FrameBufferInfo.pointer
	mbox[29] = 0;	 // FrameBufferInfo.size

	/*
  Get pitch
  Tag: 0x00040008
  Request:
  Length: 0
  Response:
  Length: 4
  Value:
  u32: bytes per line
  */
	mbox[30] = MBOX_TAG_GET_PITCH; // get pitch
	mbox[31] = 4;
	mbox[32] = MBOX_TAG_REQUEST_CODE;
	mbox[33] = 0; // FrameBufferInfo.pitch
	mbox[34] = MBOX_TAG_LAST;

	// this might not return exactly what we asked for, could be
	// the closest supported resolution instead
	if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0)
	{
		mbox[28] &= 0x3FFFFFFF; // convert GPU address to ARM address
		width = mbox[5];		// get actual physical width
		height = mbox[6];		// get actual physical height
		pitch = mbox[33];		// get number of bytes per line
		isrgb = mbox[24];		// get the actual channel order
		lfb = (void *)((unsigned long)mbox[28]);
	}
	else
	{
		uart_puts("Unable to set screen resolution to 1024x768x32\n");
	}
}

/**
 * Show a picture
 */
void lfb_showpicture()
{
	int x, y;
	unsigned char *ptr = lfb;
	char *data = homer_data, pixel[4];

	ptr += (height - homer_height) / 2 * pitch + (width - homer_width) * 2;
	for (y = 0; y < homer_height; y++)
	{
		for (x = 0; x < homer_width; x++)
		{
			HEADER_PIXEL(data, pixel);
			// the image is in RGB. So if we have an RGB framebuffer, we can copy the
			// pixels directly, but for BGR we must swap R (pixel[0]) and B (pixel[2])
			// channels.
			*((unsigned int *)ptr) =
				isrgb ? *((unsigned int *)&pixel)
					  : (unsigned int)(pixel[0] << 16 | pixel[1] << 8 | pixel[2]);
			ptr += 4;
		}
		ptr += pitch - homer_width * 4;
	}
}