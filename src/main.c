#include "uart0.h"
#include "frame_buffer.h"
#include "mbox.h"
#include "util.h"
#include "schedule.h"
#include "mm.h"
#include "exception.h"
#include "vfs.h"
#include "sdhost.h"

void boot_init() {
    // Get the amount of RAM
    mbox_vc_memory();
    mbox_arm_memory();

    // Initialize Memory
    mm_init();
    task_init();
    exc_init();

    // Initialize UART
    uart0_init();
    uart0_flush();
    uart_printf("\n[%f] Init PL011 UART done", get_timestamp());

    // Initialize Frame Buffer
    fb_init();
    uart_printf("\n[%f] Init Frame Buffer done", get_timestamp());

    // Initialize rootfs
    rootfs_init();
    uart_printf("\n[%f] Init RootFS done", get_timestamp());

    // Mount sdcard on /sdpX
    sd_init();
    uart_printf("\n[%f] Init SD done", get_timestamp());
    struct mount* sd_mps[4];
    int err = sd_mount(sd_mps);
    if (err == 0) { // success
        for (int i = 0; i < 4; i++) {
            if (sd_mps[i]) {
                uart_printf("\n%d: %s", i, sd_mps[i]->fs->name);
            }
        }
    }

    // Welcome Messages
    // fb_splash();
    uart_printf("\n\n _  _  ___ _____ _   _  ___  ___ ___ ___ \n");
    uart_printf("| \\| |/ __|_   _| | | |/ _ \\/ __|   \\_ _|\n");
    uart_printf("| .` | (__  | | | |_| | (_) \\__ \\ |) | | \n");
    uart_printf("|_|\\_|\\___| |_|  \\___/ \\___/|___/___/___|\n\n");
    mbox_board_revision();
    uart_printf("VC Core base addr: 0x%x size: 0x%x\n", vc_memory_start, vc_memory_end - vc_memory_start);
    uart_printf("ARM memory base addr: 0x%x size: 0x%x\n", arm_memory_start, arm_memory_end - arm_memory_start);
    uart_printf("\n");

    schedule_init();

    while (1) {
        schedule();
    }
}
