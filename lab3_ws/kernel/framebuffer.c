#include "io.h"
#include "kernel.h"

#define COLOR(r, g, b)                                                         \
	(isrgb ? (0x0 | r << 16 | g << 8 | b) :                                \
		 (0x0 | r << 24 | g << 16 | b << 8))

uint32_t width, height, pitch, isrgb;
uint8_t *fb;
void fb_init()
{
	uint32_t mailbox[36] = { 35 * 4,
				 MBOX_REQUEST,
				 0x48003,
				 8,
				 8,
				 1024,
				 768,
				 0x48004,
				 8,
				 8,
				 1024,
				 768,
				 0x48009,
				 8,
				 8,
				 0,
				 0,
				 0x48005,
				 4,
				 4,
				 32,
				 0x48006,
				 4,
				 4,
				 1,
				 0x40001,
				 8,
				 8,
				 4096,
				 0,
				 0x40008,
				 4,
				 4,
				 0,
				 MBOX_TAG_LAST };
	mbox_send(MBOX_CH_PROP, mailbox);
	int res = mbox_read(MBOX_CH_PROP, mailbox);
	if (res && mailbox[20] == 32 && mailbox[28] != 0) {
		mailbox[28] &= 0x3FFFFFFF;
		width = mailbox[5];
		height = mailbox[6];
		pitch = mailbox[33];
		isrgb = mailbox[24];
		fb = (void *)((uint32_t)mailbox[28]);
		printk("fb addr: %x\n", fb);
	} else {
		printk("Can't set screen to %dx%d", 1024, 768);
	}
}
void handcraft()
{
	uint32_t square_len = 32;
	uint8_t *ptr = fb;
	for (int y = 0; y < height; y++) {
		int flip = (y / square_len) % 2;
		for (int x = 0; x < width; x++) {
			*((uint32_t *)ptr) = ((x / square_len) % 2 == flip) ?
						     COLOR(255, 0, 0) :
						     COLOR(0, 0, 0);
			ptr += 4;
		}
		ptr += pitch - width * 4;
	}
}
void fb_show()
{
	handcraft();
}
