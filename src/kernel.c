/* 
 * kernel.c
 * Entry point, ker_main is executed right after the memory setup
 */

#include "uart.h"
#include "framebuffer.h"
#include "shell.h"
#include "printf.h"
#include "math.h"
#include "exception.h"
#include "time.h"

void ker_main() {

    uart_setup();
    init_framebuffer(1280, 720);
    color_fill(COLOR_SPLASH);
    position_t p1 = {0, 400};
    position_t p2 = {1280, 600};
    draw_line(p1, p2, 20, COLOR_WHITE);
    shell_start();
}
