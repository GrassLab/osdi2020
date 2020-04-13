// load exception_table to VBAR_EL2
.section ".text"
.extern exc_handler
.extern undefined_handler

// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  b undefined_handler // branch to a handler function.
  .align 7 // entry size is 0x80, .align will pad 0
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7

  b exc_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7

  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7

  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7
  b undefined_handler
  .align 7

