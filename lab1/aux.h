#include "meta_macro.h"
#include <stdint.h>

#ifndef __AUX_H__
#define __AUX_H__

// little-endian, all registers seem to align to 4 byte
//
#define AUX_IRQ            ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215000))) /* Auxiliary Interrupt status 3 */
#define AUX_ENABLES        ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215004))) /* Auxiliary enables 3 */
#define AUX_MU_IO_REG      ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215040))) /* Mini Uart I/O Data 8 */
#define AUX_MU_IER_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215044))) /* Mini Uart Interrupt Enable 8 */
#define AUX_MU_IIR_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215048))) /* Mini Uart Interrupt Identify 8 */
#define AUX_MU_LCR_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E21504C))) /* Mini Uart Line Control 8 */
#define AUX_MU_MCR_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215050))) /* Mini Uart Modem Control 8 */
#define AUX_MU_LSR_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215054))) /* Mini Uart Line Status 8 */
#define AUX_MU_MSR_REG     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215058))) /* Mini Uart Modem Status 8 */
#define AUX_MU_SCRATCH     ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E21505C))) /* Mini Uart Scratch 8 */
#define AUX_MU_CNTL_REG    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215060))) /* Mini Uart Extra Control 8 */
#define AUX_MU_STAT_REG    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215064))) /* Mini Uart Extra Status 32 */
#define AUX_MU_BAUD_REG    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215068))) /* Mini Uart Baudrate 16 */
#define AUX_SPI0_CNTL0_REG ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215080))) /* SPI 1 Control register 0 32 */
#define AUX_SPI0_CNTL1_REG ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215084))) /* SPI 1 Control register 1 8 */
#define AUX_SPI0_STAT_REG  ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215088))) /* SPI 1 Status 32 */
#define AUX_SPI0_IO_REG    ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215090))) /* SPI 1 Data 32 */
#define AUX_SPI0_PEEK_REG  ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E215094))) /* SPI 1 Peek 16 */
#define AUX_SPI1_CNTL0_REG ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E2150C0))) /* SPI 2 Control register 0 32 */
#define AUX_SPI1_CNTL1_REG ((uint32_t *)(PERIPHERAL_TO_PHYSICAL(0x7E2150C4))) /* SPI 2 Control register 1 8 */

#endif
