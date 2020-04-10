// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table
exception_table:
  b not_implemented
  .align 7 // entry size is 0x80, .align will pad 0
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b sync_handler
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7

  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
  b not_implemented
  .align 7
