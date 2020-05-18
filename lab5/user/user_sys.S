.section ".text.user"

.set SYS_WRITE_NUMBER, 0        // syscal numbers

call_sys_write_user2:
   mov w8, #SYS_WRITE_NUMBER
   svc #0
   ret
