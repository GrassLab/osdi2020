// Simple vector table
.align 11 // vector table should be aligned to 0x800
.global exception_table

exception_table:
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler // branch to a handler function.
    .align 7 // entry size is 0x80, .align will pad 0
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7

    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7

    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7

    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
    //bl save_all
    //str x30, [sp, 16 * 15]
    b test_handler
    .align 7
