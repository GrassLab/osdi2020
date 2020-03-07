SECTIONS
{
	. = 0x80000;			/*location counter: express current address, that is to say, move address to 0x8000*/
	.text : { *(.text) }	/*put the .text input section of all the files into this place*/
}

/*
	Setting the Entry point by following order
	1. '-e' entry command-line option
	2. ENTRY(symbol) command in linker script
	3. The value of a target-specific symbol, if it is defined. Mostly start.
	4. The address of the first byte of the '.text' section, if present.
	5. The address 0.
*/
