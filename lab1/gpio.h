#include "meta_macro.h"
#include <stdint.h>

#ifndef __GPIO_H__
#define __GPIO_H__

#define GPFSEL0   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200000))) /* GPIO Function Select 0 32 R/W */
#define GPFSEL1   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200004))) /* GPIO Function Select 1 32 R/W */
#define GPFSEL2   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200008))) /* GPIO Function Select 2 32 R/W */
#define GPFSEL3   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20000C))) /* GPIO Function Select 3 32 R/W */
#define GPFSEL4   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200010))) /* GPIO Function Select 4 32 R/W */
#define GPFSEL5   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200014))) /* GPIO Function Select 5 32 R/W */
#define GPSET0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20001C))) /* GPIO Pin Output Set 0 32 W */
#define GPSET1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200020))) /* GPIO Pin Output Set 1 32 W */
#define GPCLR0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200028))) /* GPIO Pin Output Clear 0 32 W */
#define GPCLR1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20002C))) /* GPIO Pin Output Clear 1 32 W */
#define GPLEV0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200034))) /* GPIO Pin Level 0 32 R */
#define GPLEV1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200038))) /* GPIO Pin Level 1 32 R */
#define GPEDS0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200040))) /* GPIO Pin Event Detect Status 0 32 R/W */
#define GPEDS1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200044))) /* GPIO Pin Event Detect Status 1 32 R/W */
#define GPREN0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20004C))) /* GPIO Pin Rising Edge Detect Enable 0 32 R/W */
#define GPREN1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200050))) /* GPIO Pin Rising Edge Detect Enable 1 32 R/W */
#define GPFEN0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200058))) /* GPIO Pin Falling Edge Detect Enable 0 32 R/W */
#define GPFEN1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20005C))) /* GPIO Pin Falling Edge Detect Enable 1 32 R/W */
#define GPHEN0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200064))) /* GPIO Pin High Detect Enable 0 32 R/W */
#define GPHEN1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200068))) /* GPIO Pin High Detect Enable 1 32 R/W */
#define GPLEN0    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200070))) /* GPIO Pin Low Detect Enable 0 32 R/W */
#define GPLEN1    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200074))) /* GPIO Pin Low Detect Enable 1 32 R/W */
#define GPAREN0   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20007C))) /* GPIO Pin Async. Rising Edge Detect 0 32 R/W */
#define GPAREN1   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200080))) /* GPIO Pin Async. Rising Edge Detect 1 32 R/W */
#define GPAFEN0   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200088))) /* GPIO Pin Async. Falling Edge Detect 0 32 R/W */
#define GPAFEN1   ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20008C))) /* GPIO Pin Async. Falling Edge Detect 1 32 R/W */
#define GPPUD     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200094))) /* GPIO Pin Pull-up/down Enable 32 R/W */
#define GPPUDCLK0 ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E200098))) /* GPIO Pin Pull-up/down Enable Clock 0 32 R/W */
#define GPPUDCLK1 ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E20009C))) /* GPIO Pin Pull-up/down Enable Clock 1 32 R/W */

#endif
