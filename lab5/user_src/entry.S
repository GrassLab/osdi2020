#include "usrlib.h"

.section ".text.entry"

.global user_entry
user_entry:
	bl main
	bl exit
