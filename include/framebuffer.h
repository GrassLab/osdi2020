#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include "types.h"

enum {
    GET_FRAMEBUFFER   = 0x00040001,
    GET_PITCH         = 0x00040008,
    SET_PHY_WH        = 0x00048003,
    SET_VIR_WH        = 0x00048004,
    SET_DEPTH         = 0x00048005,
    SET_PIX_ORD       = 0x00048006,
    SET_VIR_OFF       = 0x00048009
};

typedef struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

void init_framebuffer();

void color_fill(uint8_t r, uint8_t g, uint8_t b);

void draw_picture(const uint32_t *pic, uint32_t x_size, uint32_t y_size);

#endif
