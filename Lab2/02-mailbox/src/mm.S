.globl memzero
memzero:
	str xzr, [x0], #8 //xzr=64bits zero reg-> 
			  //store 0 in x0, then update x0 to x0+8
	subs x1, x1, #8
	b.gt memzero
	ret
