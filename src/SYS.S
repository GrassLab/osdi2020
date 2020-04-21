
.globl sync_vector_init
sync_vector_init:
	adr x0, vectors
	msr vbar_el2, x0
	ret

.globl sync_call
sync_call:
    svc #1
    ret
