/*
 * Copyright (C) 2018 bzt (bztsrc@github)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */
#include "lib/ctype.h"
#include "lib/stdarg.h"
#include "lib/string.h"

#include "gpio.h"
#include "mailbox.h"
#include "uart.h"

/**
 * Set baud rate and characteristics (115200 8N1) and map to GPIO
 */
void uart_init()
{
    register unsigned int reg;

    /*  turn off UART0 */
    *UART_CR = 0;         

    mbox_set_clock_to_PL011();

    /* map UART0 to GPIO pins */
    reg  = *GPFSEL1;
    reg &= ~((7<<12)|(7<<15));  /* address of gpio 14, 15 */
    reg |=   (4<<12)|(4<<15);   /* set to alt0 */

    *GPFSEL1 = reg;
    *GPPUD = 0;                 /* enable gpio 14 and 15 */
    reg = 150;
    while ( reg-- )
    {
        asm volatile("nop"); 
    }

    *GPPUDCLK0 = (1<<14)|(1<<15);
    reg = 150;
    while ( reg-- )
    {
        asm volatile("nop");
    }
    
    *GPPUDCLK0 = 0;             /* flush GPIO setup */

    *UART_ICR = 0x7FF;          /* clear interrupts */
    *UART_IBRD = 2;             /* 115200 baud */
    *UART_FBRD = 0xB;
    *UART_LCRH = 0b11<<5;       /* 8n1 */
    *UART_CR = 0x301;           /* enable Tx, Rx, FIFO */

    //uart_flush();
}

/**
 * Send a character
 */
void uart_send(unsigned int c)
{
    /* Wait until we can send */
    do {
        
        asm volatile("nop");

    } while( *UART_FR&0x20 );
    
    /* write the character to the buffer */   
    *UART_DR = c;

    if ( c == '\n' ) 
    {
        do {
            
            asm volatile("nop");

        } while( *UART_FR&0x20 );
        
        *UART_DR = '\r';
    }
}

/**
 * Send a integer
 */
void uart_send_int ( int c, int field_length, int base)
{
    char buffer[20];

    switch ( base )
    {
        case 10:
            itoa( c, buffer, field_length );
            break;
        case 16:
            itohex_str ( c, sizeof(int), buffer );
            break;
    }
    
    uart_puts( buffer );
}

/**
 * Send a float
 */
void uart_send_float ( float f, int field_length )
{
    char buffer[20];
    ftoa ( f, buffer, 6 );
    uart_puts( buffer );
}


/**
 * Receive a character
 */
char uart_getc()
{
    char r;
    
    /* wait until something is in the buffer */
    do{
        
        asm volatile("nop");
        
    } while ( *UART_FR&0x10 );

    /* read it and return */
    r = ( char )( *UART_DR );

    /* convert carrige return to newline */
    return r == '\r' ? '\n' : r;
}

/**
 * Receive a integer
 */
int uart_getint()
{
    int input, output;

    output = 0;
    
    while ( 1 ) 
    {
        input = uart_getc();
        uart_send(input);

        if ( !isdigit ( input ) )
            break;

        output = output * 10 + (input - '0');
        
    }

    return output;
}

/**
 * Display a string
 */
void uart_puts(char *s)
{
    while( *s )
    {
        uart_send(*s++);
    }
}

void uart_printf ( const char * format, ... )
{
    const char * temp = format;
    int field_length;
    int d;
    char c;
    char *s;
    float f;
    va_list arguments;

    va_start ( arguments, format );

    do
    {
        if ( *temp == '%' )
        {
            temp ++;

            // field length
            field_length = 0;
            if (isdigit(*temp))
            {
                field_length = * temp;

                while ( isdigit(*temp) ) 
                {
                    field_length *= 10;
                    field_length += (*temp);
                    temp ++;
                }
            }

            // format different argument
            switch ( *temp )
            {
                case 'd':
                    d = va_arg ( arguments, int );
                    uart_send_int( d, field_length, 10 );
                    break;
                case 's':
                    s = va_arg ( arguments, char * );
                    uart_puts ( s );
                    break;
                case 'c':
                    c = (char) va_arg ( arguments, int );
                    uart_send ( c );
                    break;
                case 'x':
                    d = va_arg ( arguments, int ); 
                    uart_send_int( d, field_length, 16 );
                    break;
                case 'f':
                    f = va_arg ( arguments, double );
                    uart_send_float ( f, field_length );
                    break;
            }
        }
        else
        {
            uart_send( *temp );
        }

        temp ++;

    } while ( *temp != '\0');
}