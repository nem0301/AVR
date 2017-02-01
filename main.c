#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define F_CPU 16000000UL
#define BAUD 115200
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
    char text[30];
}Console;

Console console[20];
char cmdbuf[30];
// cursors position
int x, y;
int cy = -1;

void uart_init(void);
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);
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
    int i;
	switch(c)
	{
	case 13:
	    x = 0;
	    putchar('\n');

	    if (strlen(cmdbuf) > 0)
        {
            strcpy(console[y].text, cmdbuf);
            func(cmdbuf);
            y = (y + 1) % 20;
            console[y].text[0] = '\0';
            cmdbuf[0] = '\0';
        }

	    printf("%s", prompt);
	    cy = -1;
		break;
	case 8:
		if (x > 0)
        {
            x--;
            i = x;
            while (cmdbuf[i] != '\0')
            {
                cmdbuf[i] = cmdbuf[i+1];
                i++;
            }
            printf("%c[2K\r", 27);
            printf("%s%s", prompt, cmdbuf);

            int len = strlen(cmdbuf);

            for (i = 0; i < len - x; i++)
            {
                putchar(27);
                putchar(91);
                putchar('D');
            }
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

            if (cy == -1) cy = y;

            if (c == 65)
            {
                cy--;

                if (cy < 0)
                    cy = 20 + cy;

                if (console[cy].text[0] == '\0')
                {
                    cy = (cy + 1) % 20;
                }
                else
                {
                    printf("%c[2K\r", 27);
                    printf("%s%s", prompt, console[cy].text);
                    strcpy(cmdbuf, console[cy].text);
                    x = strlen(cmdbuf);
                }
            }
            else if ( c == 66)
            {
                if (console[cy].text[0] == '\0' || y - cy == 0)
                {

                }
                else
                {
                    printf("%c[2K\r", 27);
                    cy = (cy + 1) % 20;
                    printf("%s%s", prompt, console[cy].text);
                    strcpy(cmdbuf, console[cy].text);
                    x = strlen(cmdbuf);
                }
            }
            else if (c == 67)
            {
                if(x < strlen(cmdbuf))
                {
                    putchar(27);
                    putchar(91);
                    putchar('C');
                    x++;
                }
            }
            else if (c == 68)
            {
                if(x > 0)
                {
                    putchar(27);
                    putchar(91);
                    putchar('D');
                    x--;
                }
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

        if (strlen(cmdbuf) < 29)
        {
            for (i = 28; i >= x; i--)
            {
                cmdbuf[i + 1] = cmdbuf[i];
            }
            cmdbuf[x] = c;
            x++;
            printf("%c[2K\r", 27);
            printf("%s%s", prompt, cmdbuf);
            int len = strlen(cmdbuf);

            for (i = 0; i < len - x; i++)
            {
                putchar(27);
                putchar(91);
                putchar('D');
            }
        }

		break;
	}
}

#define CMD_NUM     6
char *cmdList[CMD_NUM] =
{
    "history",
    "clear",
    "ls",
    "spi",
    "rwb",
    "rww"
};

void func(char *args)
{
    char *token;
    char *rest;
    char str[30];
    char *argv[10];
    int argc = 0;
    int i, j;

    memset(str, '\0', sizeof(str));
    strcpy(str, args);
    rest = str;

    while((token = strtok_r(rest, " ", &rest)))
    {
        int len = strlen(token);
        argv[argc] = (char*) malloc (len + 1);
        strcpy(argv[argc], token);
        argv[argc][len] = '\0';
        argc++;
    }

    char *cmd = argv[0];

    if (strcmp(cmd, "history") == 0)
    {
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
        for (i = 0; i < CMD_NUM; i++)
        {
            printf("%10s", cmdList[i]);
        }
        putchar('\n');
    }
    else if (strcmp(cmd, "rwb") == 0 || strcmp(cmd, "rww") == 0)
    {
        if (argc < 3)
        {
            printf("usage rw(b|w) (-r|-w) addr [value]\n");
        }
        else
        {
            int addr = (int)strtol(argv[2], NULL, 0);
            uint8_t byte;
            uint16_t word;

            if (strcmp(argv[1], "-r") == 0)
            {
                if (strcmp(cmd, "rwb") == 0)
                {
                    byte = eeprom_read_byte((uint8_t*)addr);
                    printf("addr : 0x%04x,  value : 0x%02x\n", addr, byte);
                }
                else if (strcmp(cmd, "rww") == 0)
                {
                    word = eeprom_read_word((uint16_t*)addr);
                    printf("addr : 0x%04x,  value : 0x%04x\n", addr, word);
                }

            }
            else if (strcmp(argv[1], "-w") == 0)
            {
                if (argc < 4)
                {
                    printf("usage rw(b|w) (-r|-w) hexAddr [hexValue]\n");
                }
                else
                {
                    if (strcmp(cmd, "rwb") == 0)
                    {
                        byte = (uint8_t)strtol(argv[3], NULL, 0);
                        eeprom_write_byte((uint8_t*)addr, (uint8_t)byte);
                        byte = eeprom_read_byte((uint8_t*)addr);
                        printf("addr : 0x%04x,  value : 0x%02x\n", addr, byte);
                    }
                    else if (strcmp(cmd, "rww") == 0)
                    {
                        word = (uint16_t)strtol(argv[3], NULL, 0);
                        eeprom_write_word((uint16_t*)addr, (uint16_t)word);
                        word = eeprom_read_word((uint16_t*)addr);
                        printf("addr : 0x%04x,  value : 0x%04x\n", addr, word);
                    }
                }
            }


        }
    }
    #ifdef MASTER
    else if (strcmp(cmd, "spi") == 0)
    {
        if (argc < 2)
        {
            printf("usage spi number\n");
            for (i = 0; i < argc; i++)
            {
                free(argv[i]);
            }
            return;
        }

        for (i = 0; i < strlen(argv[1]); i++)
        {
            if (argv[1][i] < '0' || argv[1][i] > '9')
            {
                printf("arg format error : %s \n", argv[1]);
                for (i = 0; i < argc; i++)
                {
                    free(argv[i]);
                }
                return;
            }
        }

        printf("ret %d\n", masterWrite(atoi(argv[1])));
    }
    #endif // MASTER
    else
    {
        printf("undefined command \"%s\"\n", cmd);
    }

    for (i = 0; i < argc; i++)
    {
        free(argv[i]);
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
