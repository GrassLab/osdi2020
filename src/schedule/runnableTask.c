void idleTask()
{
	while (1)
	{
		schedule();
	}
}

void task1()
{
    while (1)
    {
        uartInt(1);
        uartPuts("...\n");
        schedule();
        for (int i = 0; i < 15000000; ++i)
        {
            asm volatile("nop");
        }
    }
}

void task2()
{
    while (1)
    {
        uartInt(2);
        uartPuts("...\n");
        schedule();
        for (int i = 0; i < 15000000; ++i)
        {
            asm volatile("nop");
        }
    }
}