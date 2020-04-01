/* 
 * kernel.c
 * Entry point, ker_main is executed right after the memory setup
 */

#include "uart.h"
#include "framebuffer.h"
#include "shell.h"
#include "splash.h"

void ker_main() {

    uart_setup();
    init_framebuffer();
    color_fill(40, 30, 35);
    draw_picture(_acsg_logo_i, 400, 385);
    shell_start();
}
