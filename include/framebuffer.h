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


typedef struct position {
    uint32_t x;
    uint32_t y;
} position_t;

typedef struct pixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} pixel_t;

#define COLOR_BLACK     (pixel_t){0, 0, 0}
#define COLOR_WHITE     (pixel_t){255, 255, 255}
#define COLOR_RED       (pixel_t){255, 0, 0}
#define COLOR_GREEN     (pixel_t){0, 255, 0}
#define COLOR_BLUE      (pixel_t){0, 0, 255}
#define COLOR_SPLASH    (pixel_t){40, 30, 35}


void init_framebuffer(uint32_t w, uint32_t h);

void color_fill(pixel_t color);

void draw_line(position_t p1, position_t p2, uint32_t thickness, pixel_t color);

void draw_circle(position_t center, uint32_t r, pixel_t color);

void draw_picture(const uint32_t *pic, uint32_t x_size, uint32_t y_size);

#endif
