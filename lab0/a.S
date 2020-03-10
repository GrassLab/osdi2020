.section ".text"	//The .section directive switches the current tareget section to the one described by its arguments. That is to say, switch to .text section.
_start:
	wfe				//Wait for event. Let ARM kerenl enter low-power standby mode.
	b _start		//unconditional branch.
