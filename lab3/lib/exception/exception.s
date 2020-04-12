.section ".text.exception"

.align 11 // aligned to 0x800 (2^11)
.global _exception_table
_exception_table:
  b _exception_handler // branch to a handler function.
  .align 7 // entry size is 0x80 (2^7), .align will pad 0
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7

  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7

  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7

  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7
  b _exception_handler
  .align 7 

.global _exception_handler
_exception_handler:
