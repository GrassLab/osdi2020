#include "printf.h"
#include "uart.h"

// we directly discart the significand reserved bits of esr
void sync_router(unsigned long esr, unsigned long elr, unsigned long spsr, unsigned long far){
    switch(esr>>26) {
        case 0b010101: sync_svc_handler(esr, elr); break;
        default: uart_puts("Unknown sync"); break;
    }
}

void sync_svc_handler(unsigned long esr, unsigned long elr){
    unsigned long iss = esr&0xFFFFFF;

    switch(esr&0xFFFFFF) {
        case 1: 
            printf("Exception (ELR): 0x%016X\n", elr);
            printf("Exception (EC) : 0x%02X\n", esr>>26);
            printf("Exception (ISS): 0x%06X\n", esr&0xFFFFFF);
            break;
        default: uart_puts("Unknown SVC"); break;
    }
}