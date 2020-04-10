#ifndef EXCEPTION_H_
#define EXCEPTION_H_

extern unsigned exception_table[];

#define save_all()                    \
  do {                                \
    asm(                              \
      "sub sp, sp, 32 * 8\n"          \
      "stp x0, x1, [sp ,16 * 0]\n"    \
      "stp x2, x3, [sp ,16 * 1]\n"    \
      "stp x4, x5, [sp ,16 * 2]\n"    \
      "stp x6, x7, [sp ,16 * 3]\n"    \
      "stp x8, x9, [sp ,16 * 4]\n"    \
      "stp x10, x11, [sp ,16 * 5]\n"  \
      "stp x12, x13, [sp ,16 * 6]\n"  \
      "stp x14, x15, [sp ,16 * 7]\n"  \
      "stp x16, x17, [sp ,16 * 8]\n"  \
      "stp x18, x19, [sp ,16 * 9]\n"  \
      "stp x20, x21, [sp ,16 * 10]\n" \
      "stp x22, x23, [sp ,16 * 11]\n" \
      "stp x24, x25, [sp ,16 * 12]\n" \
      "stp x26, x27, [sp ,16 * 13]\n" \
      "stp x28, x29, [sp ,16 * 14]\n" \
      "str x30, [sp, 16 * 15]\n"      \
    );                                \
  } while (0)

#define load_all()                    \
  do {                                \
    asm(                              \
      "ldp x0, x1, [sp ,16 * 0]\n"    \
      "ldp x2, x3, [sp ,16 * 1]\n"    \
      "ldp x4, x5, [sp ,16 * 2]\n"    \
      "ldp x6, x7, [sp ,16 * 3]\n"    \
      "ldp x8, x9, [sp ,16 * 4]\n"    \
      "ldp x10, x11, [sp ,16 * 5]\n"  \
      "ldp x12, x13, [sp ,16 * 6]\n"  \
      "ldp x14, x15, [sp ,16 * 7]\n"  \
      "ldp x16, x17, [sp ,16 * 8]\n"  \
      "ldp x18, x19, [sp ,16 * 9]\n"  \
      "ldp x20, x21, [sp ,16 * 10]\n" \
      "ldp x22, x23, [sp ,16 * 11]\n" \
      "ldp x24, x25, [sp ,16 * 12]\n" \
      "ldp x26, x27, [sp ,16 * 13]\n" \
      "ldp x28, x29, [sp ,16 * 14]\n" \
      "ldr x30, [sp, 16 * 15]\n"      \
      "add sp, sp, 32 * 8\n"          \
    );                                \
  } while (0)

void exception_init(void);
void sync_handler(void);

#endif // EXCEPTION_H_
