#include <avr/io.h>
#include <util/setbaud.h>
#include "uart.h"

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void uart_init(void)
{
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);	// bxxxx xx1x
#else
	UCSR0A &= ~(_BV(U2X0));	// bxxxx xx0x
#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */

    // binding uart get/put to stdio
	stdout = &uart_output;
	stdin  = &uart_input;
}

int uart_putchar(char c, FILE *stream)
{
    if (c == '\n')
	{
		uart_putchar('\r', stream);
	}

	loop_until_bit_is_set(UCSR0A, UDRE0);

    UDR0 = c;
    return c;
}

int uart_getchar(FILE *stream)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return (char)UDR0;
}

