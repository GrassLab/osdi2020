#include "uart.h"
#include "mailbox.h"
#include "string.h"
#include "framebuffer.h"

unsigned int width, height, pitch;
unsigned char *framebuffer;

void init_framebuffer() {
    
    uart_log(LOG_INFO, "Initializing the framebuffer");

    volatile unsigned int __attribute__((aligned(16))) mbox[35] = {
        35*4,
        REQUEST_CODE,
        SET_PHY_WH, 8, TAG_REQUEST_CODE, 1280, 720,
        SET_VIR_WH, 8, TAG_REQUEST_CODE, 1280, 720,
        SET_VIR_OFF, 8, TAG_REQUEST_CODE, 0, 0,
        SET_DEPTH, 4, TAG_REQUEST_CODE, 24,
        SET_PIX_ORD, 4, TAG_REQUEST_CODE, 1,
        GET_FRAMEBUFFER, 8, TAG_REQUEST_CODE, 4096, 0,
        GET_PITCH, 4, TAG_REQUEST_CODE, 0,
        END_TAG
    };

    if(mbox_call(mbox, MBOX_CH_PROP) && mbox[28] != 0) {
        uart_log(LOG_INFO, "Framebuffer initialized successfully");
        mbox[28]    &= 0x3FFFFFFF;      //convert GPU address to ARM address
        width       = mbox[5];          //get actual physical width
        height      = mbox[6];          //get actual physical height
        pitch       = mbox[33];         //get number of bytes per line
        framebuffer = (void*)((uint32_t)mbox[28]);
    } else {
        uart_log(LOG_ERROR, "Unable to initialize the framebuffer");
    }
}

static void write_pixel(uint32_t x, uint32_t y, pixel_t *pix) {
    uint8_t *location = framebuffer + y*pitch + x*3;
    memcpy(location, pix, 3);
}

void color_fill(uint8_t r, uint8_t g, uint8_t b) {
    pixel_t p = {r, g, b};
    for (size_t i = 0; i < width; i++) {
        for (size_t j = 0; j < height; j++) {
            write_pixel(i, j, &p);
        }
    }
}

void draw_picture(const uint32_t *pic, uint32_t x_size, uint32_t y_size) {
    for (uint32_t i = 0; i < x_size * y_size; i++) {
        pixel_t *p = (pixel_t *)&pic[i];
        if ((pic[i] & 0xFF000000) != 0xFF000000) // don't draw transparent pixels
            write_pixel(
                    (((width - x_size) / 2) + (i % x_size)),
                    (((height - y_size) / 2) + (i / y_size)),
                    p
            );
            // write_pixel(i%x_size, i/y_size, p);
    }
}
