.global set32
set32:
	str w1, [x0]
	ret

.global get32
get32:
	ldr w0, [x0]
	str w0, [x1]
	ret
