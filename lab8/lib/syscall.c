char uart_read()
{
	char ret;
	asm volatile("mov x0, #16\n\t"
		     "svc #0");
	asm volatile("mov %0, x0" : "=r"(ret)::);
	return ret;
}

void uart_write(char *c)
{
	asm volatile("mov x1, %0\n\t"
		     "mov x0, #17\n\t"
		     "svc #0" ::"r"(c)
		     :);
}