#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define BAUD 57600

#include <util/setbaud.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MASTER


// SPI
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB

#define SCK         PORTB5
#define MISO        PORTB4
#define MOSI        PORTB3
#define SS          PORTB2

static char esc = 0;
static char lbracket = 0;

char prompt[20] = "console > ";

typedef struct {
    char text[80];
}Console;

Console console[20];
// cursors position
int x, y;

void uart_init(void);
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);
void clearScreen();
void setCursor(int x, int y);
void specialKey(char c);
void func(char *cmd);

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

void SPI_init()
{
#ifdef MASTER
    SPI_PORT |= _BV(SS);
    SPI_DDR |= (_BV(SS) | _BV(SCK) | _BV(MOSI));
    SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR0)); // 16M/16=1Mhz
#else
    SPI_DDR |= _BV(MISO);
    SPCR = _BV(SPE);
#endif // MASTER
}

unsigned char masterWrite(unsigned char data)
{
    SPI_PORT &= ~_BV(SS);
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}

unsigned char slaveRead(unsigned char prev)
{
    SPDR = prev;
    while (!(SPSR & _BV(SPIF)));

    return SPDR;
}

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
	printf("\033[H\033[J");
	esc = 0;
    lbracket = 0;
}

void setCursor(int x, int y)
{
	printf("\033[%d;%dH", (y)+1, (x)+1);
}

void key(char c)
{


	switch(c)
	{
	case 13:
	    x = 0;
	    putchar('\n');

	    if (strlen(console[y].text) > 0)
        {
            func(console[y].text);
            y = (y + 1) % 20;
            memset(console[y].text, '\0', sizeof(console[y].text));
        }

	    printf("%s", prompt);
		break;
	case 8:
		if (x > 0)
        {
            putchar('\b');
            putchar(' ');
            x--;
            putchar(c);
        }
		break;
    default:
        if (!esc && !lbracket && c == 27)
        {
            esc = 1;
            putchar(c);
            return;
        }
        else if (esc && !lbracket && c == 91)
        {
            lbracket = 1;
            putchar(c);
            return;
        }
        else if (esc && lbracket && c > 64 && c < 69)
        {
            switch(c)
            {
            case 65:
            case 66:
            case 67:
            case 68:
            default:
                putchar('C');
                putchar(27);
                putchar(91);
                putchar('D');
            }

            esc = 0;
            lbracket = 0;
            return;
        }
        else
        {
            esc = 0;
            lbracket = 0;
        }

	    console[y].text[x] = c;
		x++;
		putchar(c);
		console[y].text[x] = '\0';
		break;
	}
}

#define CMD_NUM     4
char *cmdList[CMD_NUM] =
{
    "history",
    "clear",
    "ls",
    "set"
};

void func(char *cmd)
{
    if (strcmp(cmd, "history") == 0)
    {
        int i = 0;
        int j = 0;
        for (j = 0, i = y + 1; j <20 ; i++, j++)
        {
            printf("%2d \t %s\n", j + 1, console[i%20].text);
        }
    }
    else if (strcmp(cmd, "clear") == 0)
    {
        clearScreen();
    }
    else if (strcmp(cmd, "ls") == 0)
    {
        int i= 0;
        for (i = 0; i < CMD_NUM; i++)
        {
            printf("%10s", cmdList[i]);
        }
        putchar('\n');
    }
    #ifdef MASTER
    else if (strcmp(cmd, "set 1") == 0)
    {
        printf("set 1\n");
        printf("ret %d\n", masterWrite(1));
    }
    else if (strcmp(cmd, "set 0") == 0)
    {
        printf("set 0\n");
        printf("ret %d\n", masterWrite(0));
    }
    #endif // MASTER
    else
    {
        printf("undefine comman \"%s\"\n", cmd);
    }

}

int main(void)
{
    SPI_init();
	uart_init();
	stdout = &uart_output;
	stdin  = &uart_input;

	clearScreen();
	printf("%s", prompt);

#ifndef MASTER
    unsigned char n = 0;
#endif // MASTER

    for(;;){
        /* insert your main loop code here */

        #ifdef MASTER
        char c = getchar();
		key(c);
		//printf("%d\n", c);
		#else
		n = slaveRead(n);
		printf("%d\n", n);
		#endif
    }
    return 0;   /* never reached */
}
