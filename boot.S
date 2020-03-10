.section ".text"
_start:
  and x0, x0, #0xff
  and x1, x1, #0xff
  and x2, x2, #0xff
  and x3, x3, #0xff
  b _start
//.globl _start
//_start:
//  nop
//  nop
//  nop
//  nop
//  nop
//  nop
//  nop
//    mrs    x0, mpidr_el1        
//    and    x0, x0,#0xFF        // Check processor id
//    cbz    x0, master        // Hang for all non-primary CPU
//    b    proc_hang
//
//proc_hang: 
//    b proc_hang
//
//master:
//    b master
