#include "stdint.h"
#include "uart.h"

char *Exception[] = {
	"Exception_Uncategorized", // Uncategorized or unknown reason
	"Exception_WFxTrap", // Trapped WFI or WFE instruction
	"Exception_CP15RTTrap", // Trapped AArch32 MCR or MRC access to CP15
	"Exception_CP15RRTTrap", // Trapped AArch32 MCRR or MRRC access to CP15
	"Exception_CP14RTTrap", // Trapped AArch32 MCR or MRC access to CP14
	"Exception_CP14DTTrap", // Trapped AArch32 LDC or STC access to CP14
	"Exception_AdvSIMDFPAccessTrap", // HCPTR-trapped access to SIMD or FP
	"Exception_FPIDTrap", // Trapped access to SIMD or FP ID register
	// Trapped BXJ instruction not supported in Armv8
	"Exception_PACTrap", // Trapped invalid PAC use
	"Exception_CP14RRTTrap", // Trapped MRRC access to CP14 from AArch32
	"Exception_IllegalState", // Illegal Execution state
	"Exception_SupervisorCall", // Supervisor Call
	"Exception_HypervisorCall", // Hypervisor Call
	"Exception_MonitorCall", // Monitor Call or Trapped SMC instruction
	"Exception_SystemRegisterTrap", // Trapped MRS or MSR system register access
	"Exception_ERetTrap", // Trapped invalid ERET use
	"Exception_InstructionAbort", // Instruction Abort or Prefetch Abort
	"Exception_PCAlignment", // PC alignment fault
	"Exception_DataAbort", // Data Abort
	"Exception_NV2DataAbort", // Data abort at EL1 reported as being from EL2
	"Exception_SPAlignment", // SP alignment fault
	"Exception_FPTrappedException", // IEEE trapped FP Exception
	"Exception_SError", // SError interrupt
	"Exception_Breakpoint", // (Hardware) Breakpoint
	"Exception_SoftwareStep", // Software Step
	"Exception_Watchpoint", // Watchpoint
	"Exception_SoftwareBreakpoint", // Software Breakpoint Instruction
	"Exception_VectorCatch", // AArch32 Vector Catch
	"Exception_IRQ", // IRQ interrupt
	"Exception_SVEAccessTrap", // HCPTR trapped access to SVE
	"Exception_BranchTarget", // Branch Target Identification
	"Exception_FIQ" // FIQ interrupt
};

#define set(a, b) (*a = b)
#define get(a, b) (b = *a)
void arm_timer_handler()
{
#define ARM_TIMER_IRQ_CLR (uint32_t *)0x3f00b40c
	set(ARM_TIMER_IRQ_CLR, 1);
}
void local_timer_handler()
{
#define LOCAL_TIMER_IRQ (uint32_t *)0x40000038
	set(LOCAL_TIMER_IRQ, 0xC0000000); // clear interrupt and reload.
	return;
}

void sys_timer_handler()
{
	unsigned int t;
#define SYSTEM_TIMER_COMPARE1 (uint32_t *)0x3f003010
#define SYSTEM_TIMER_CLO (uint32_t *)0x3f003004
#define SYSTEM_TIMER_CS (uint32_t *)0x3f003000
	get(SYSTEM_TIMER_CLO, t);
	set(SYSTEM_TIMER_COMPARE1, t + 2500000);
	set(SYSTEM_TIMER_CS, 0xf);
}

void show_exc()
{
	unsigned long current_el, esr, elr;
	asm volatile("mrs %0, CurrentEl" : "=r"(current_el)::);

	switch (current_el >> 2) {
	case 0x0:
		uart_puts("Running at EL0");
		return;
	case 0x1:
		uart_puts("Running at EL1");
		asm volatile("mrs %0, esr_el1\n\t"
			     "mrs %1, elr_el1"
			     : "=r"(esr), "=r"(elr)::);
		break;
	case 0x2:
		uart_puts("Running at EL2");
		asm volatile("mrs %0, esr_el2\n\t"
			     "mrs %1, elr_el2"
			     : "=r"(esr), "=r"(elr)::);
		break;
	case 0x3:
		uart_puts("Running at EL3");
		asm volatile("mrs %0, esr_el3\n\t"
			     "mrs %1, elr_el3"
			     : "=r"(esr), "=r"(elr)::);
		break;
	default:
		uart_puts("Running at unknown EL\n");
		return;
	}
	uart_puts("\nException return address: ");
	uart_hex(elr);
	uart_puts("\nException class (EC): ");
	uart_hex(esr >> 26);
	uart_puts("\nInstruction specific syndrome (ISS) ");
	uart_hex(esr & 0x01FFFFFF);
	uart_puts("\n");
}
