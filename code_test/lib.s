# 1 "code_test/lib.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/aarch64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "code_test/lib.S"
.section ".text.entry"

.global _user_entry
_user_entry:
 bl main
 bl exit
