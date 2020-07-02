#include "syscall.h"

.global fork
fork: 
	mov x8, DO_FORK
	svc #0
	ret