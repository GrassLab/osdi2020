
float get_current_timestamp ( )
{
    unsigned long int cnt_freq, cnt_tpct;

    asm volatile(
        "mrs %0, cntfrq_el0 \n\t"
        "mrs %1, cntpct_el0 \n\t"
        : "=r"( cnt_freq ), "=r"( cnt_tpct )
        : );

    return ( (float) cnt_tpct ) / cnt_freq;
}

void wait_cycles ( unsigned int n )
{
    if ( n )
    {
        while ( n-- )
        {
            asm volatile( "nop" );
        }
    }
}

void wait_msec ( unsigned int n )
{
    register unsigned long f, t, r;

    // get the current counter frequency
    asm volatile( "mrs %0, cntfrq_el0" : "=r"( f ) );

    // read the current counter
    asm volatile( "mrs %0, cntpct_el0" : "=r"( t ) );

    // calculate expire value for counter
    t += ( ( f / 1000 ) * n ) / 1000;
    do
    {
        asm volatile( "mrs %0, cntpct_el0" : "=r"( r ) );

    } while ( r < t );
}