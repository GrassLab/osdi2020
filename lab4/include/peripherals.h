#ifndef _PERIPHERALS_H
#define _PERIPHERALS_H

//	 _SYSREGS_H

// ***************************************
// SCTLR_EL1, System Control Register (EL1), Page 2654 of AArch64-Reference-Manual.
// ***************************************

#define SCTLR_RESERVED                  (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
#define SCTLR_EE_LITTLE_ENDIAN          (0 << 25)
#define SCTLR_EOE_LITTLE_ENDIAN         (0 << 24)
#define SCTLR_I_CACHE_DISABLED          (0 << 12)
#define SCTLR_D_CACHE_DISABLED          (0 << 2)
#define SCTLR_MMU_DISABLED              (0 << 0)
#define SCTLR_MMU_ENABLED               (1 << 0)

#define SCTLR_VALUE_MMU_DISABLED	(SCTLR_RESERVED | SCTLR_EE_LITTLE_ENDIAN | SCTLR_I_CACHE_DISABLED | SCTLR_D_CACHE_DISABLED | SCTLR_MMU_DISABLED)

// ***************************************
// HCR_EL2, Hypervisor Configuration Register (EL2), Page 2487 of AArch64-Reference-Manual.
// ***************************************

#define HCR_RW				(1 << 31)
#define HCR_VALUE			HCR_RW

// ***************************************
// SCR_EL3, Secure Configuration Register (EL3), Page 2648 of AArch64-Reference-Manual.
// ***************************************

#define SCR_RESERVED		(3 << 4)
#define SCR_RW				(1 << 10)
#define SCR_NS				(1 << 0)
#define SCR_VALUE			(SCR_RESERVED | SCR_RW | SCR_NS)

// ***************************************
// SPSR_EL3, Saved Program Status Register (EL3) Page 389 of AArch64-Reference-Manual.
// ***************************************

#define SPSR_MASK_ALL 			(7 << 6)
#define SPSR_EL1h			(5 << 0)
#define SPSR_VALUE			(SPSR_MASK_ALL | SPSR_EL1h)

//	_BASE_H

#define PBASE 0x3F000000

//	_GPIO_H
/*
#define GPFSEL1         (PBASE+0x00200004)
#define GPSET0          (PBASE+0x0020001C)
#define GPCLR0          (PBASE+0x00200028)
#define GPPUD           (PBASE+0x00200094)
#define GPPUDCLK0       (PBASE+0x00200098)
*/
//	_IRQ_H
// ARM Interrupt register
#define IRQ_BASIC_PENDING	(PBASE+0x0000B200)
#define IRQ_PENDING_1		(PBASE+0x0000B204)
#define IRQ_PENDING_2		(PBASE+0x0000B208)
#define FIQ_CONTROL			(PBASE+0x0000B20C)
#define ENABLE_IRQS_1		(PBASE+0x0000B210)
#define ENABLE_IRQS_2		(PBASE+0x0000B214)
#define ENABLE_BASIC_IRQS	(PBASE+0x0000B218)
#define DISABLE_IRQS_1		(PBASE+0x0000B21C)
#define DISABLE_IRQS_2		(PBASE+0x0000B220)
#define DISABLE_BASIC_IRQS	(PBASE+0x0000B224)

#define SYSTEM_TIMER_IRQ_0	(1 << 0)
#define SYSTEM_TIMER_IRQ_1	(1 << 1)
#define SYSTEM_TIMER_IRQ_2	(1 << 2)
#define SYSTEM_TIMER_IRQ_3	(1 << 3)

// 
#define CORE0_IRQ_SOURCE	0x40000060
#define CORE1_IRQ_SOURCE	0x40000064
#define CORE2_IRQ_SOURCE	0x40000068
#define CORE3_IRQ_SOURCE	0x4000006C


//	_MINIUART_H

#define AUX_ENABLES     (PBASE+0x00215004)
#define AUX_MU_IO_REG   (PBASE+0x00215040)
#define AUX_MU_IER_REG  (PBASE+0x00215044)
#define AUX_MU_IIR_REG  (PBASE+0x00215048)
#define AUX_MU_LCR_REG  (PBASE+0x0021504C)
#define AUX_MU_MCR_REG  (PBASE+0x00215050)
#define AUX_MU_LSR_REG  (PBASE+0x00215054)
#define AUX_MU_MSR_REG  (PBASE+0x00215058)
#define AUX_MU_SCRATCH  (PBASE+0x0021505C)
#define AUX_MU_CNTL_REG (PBASE+0x00215060)
#define AUX_MU_STAT_REG (PBASE+0x00215064)
#define AUX_MU_BAUD_REG (PBASE+0x00215068)

// _TIMER_H

#define TIMER_CS        (PBASE+0x00003000)
#define TIMER_CLO       (PBASE+0x00003004)
#define TIMER_CHI       (PBASE+0x00003008)
#define TIMER_C0        (PBASE+0x0000300C)
#define TIMER_C1        (PBASE+0x00003010)
#define TIMER_C2        (PBASE+0x00003014)
#define TIMER_C3        (PBASE+0x00003018)

#define TIMER_CS_M0	(1 << 0)
#define TIMER_CS_M1	(1 << 1)
#define TIMER_CS_M2	(1 << 2)
#define TIMER_CS_M3	(1 << 3)

#endif
