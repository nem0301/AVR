/* Name: main.c
 * Author: <insert your name here>
 * Copyright: <insert your copyright message here>
 * License: <insert your license reference here>
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>

// cursors position
int x, y;

void uart_init(void);
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);
void clearScreen();
void setCursor(int x, int y);
void specialKey(char c);

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
}

void uart_putchar(char c, FILE *stream) 
{
	if (c == '\n')
	{
		uart_putchar('\r', stream);
	}

	loop_until_bit_is_set(UCSR0A, UDRE0); 

	UDR0 = c;
}

char uart_getchar(FILE *stream)
{
	loop_until_bit_is_set(UCSR0A, RXC0);
	return UDR0;
}

void clearScreen()
{
	x = y = 1;
	setCursor(x, y);

	printf("\033[H\033[J");
}

void setCursor(int x, int y)
{
	printf("\033[%d;%dH", (y), (x));
}

void specialKey(char c)
{
	switch(c)
	{
	case 13:
		setCursor(1, ++y);
		x = 1;
		break;
	case 8:
		putchar('\b');
		putchar(' ');
		x--;
		if (x == 0)
		{
			x = 30;
			setCursor(x, --y);
		}
		break;
	default:
		x++;
		break;
	}
}

int main(void)
{
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	clearScreen();
	setCursor(x, y);

    for(;;){
        /* insert your main loop code here */

		char c = getchar();
		specialKey(c);

		//printf("%d\n", c);
		putchar(c);

		if ((PORTB & _BV(PORTB5)) == 0)
		{
			PORTB |= _BV(PORTB5);
		}
		else
		{
			PORTB &= ~_BV(PORTB5);
		}
    }
    return 0;   /* never reached */
}
