#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/setbaud.h>

#ifndef SPI_MASTER
#include <avr/interrupt.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <common.h>
#include <uart.h>
#include <spi.h>

#ifdef SPI_MASTER
//global variables
extern char esc;
extern char lbracket;

extern char prompt[20];

typedef struct {
    char text[CMDSIZE];
}Console;

extern Console console[HISTSIZE];
extern char cmdbuf[CMDSIZE];

// cursors position
extern int x, y;
extern int cy;

void clearScreen();
void setCursor(int x, int y);
void key(char c);

#define CMD_NUM 3
char *cmdList[CMD_NUM] =
{
    "clear",
    "ls",
    "spi",
};

void func(char *args)
{
    char *token;
    char *rest;
    char str[CMDSIZE];
    char *argv[10];
    int argc = 0;
    int i;

    memset(str, '\0', sizeof(str));
    strcpy(str, args);
    rest = str;

    // tokenizing
    while((token = strtok_r(rest, " ", &rest)))
    {
        int len = strlen(token);
        argv[argc] = (char*) malloc (len + 1);
        strcpy(argv[argc], token);
        argv[argc][len] = '\0';
        argc++;
    }

    char *cmd = argv[0];

    // processing command
    
	
	// clear screen
    if (strcmp(cmd, "clear") == 0)
    {
        clearScreen();
    }
    // check command
    else if (strcmp(cmd, "ls") == 0)
    {
        for (i = 0; i < CMD_NUM; i++)
        {
            printf("%10s", cmdList[i]);
        }
        putchar('\n');
    }
	else if (strcmp(cmd, "spi") == 0)
	{
		unsigned char spi_data;
		if (argc == 3)
		{
			spiTranceive(atoi(argv[1]), atoi(argv[2]), &spi_data);
		}
		else if (argc == 4)
		{
			spi_data = atoi(argv[3]);
			spiTranceive(atoi(argv[1]), atoi(argv[2]), &spi_data);
		}
		else
		{
			printf("format err : spi inst op [data]\n");
		}

	}
    // error handling
    else
    {
        printf("undefined command \"%s\"\n", cmd);
    }

    // free memory
    for (i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
}
#else
// state -1 : ready, 0 : instruction, 1 : operation, 2 : data
unsigned char cmd_buf[3];
int cmd_state = -1;

// inst, op enum
enum {
	INST_VER,
};



ISR (SPI_STC_vect)
{
	cli();

	unsigned char data = SPDR;
	printf("%02x ", data);

	if (data == 0xff && cmd_state != 2)
	{
		cmd_state = 0;
	}
	else
	{
		cmd_buf[cmd_state] = data;
		switch (cmd_state)
		{
		// inst
		case 0:

			break;
		// op
		case 1:

			break;
		// data
		case 2:


			printf("\n");
			break;
		}

		cmd_state++;
	}

	sei();
}
#endif


int main(void)
{
    // UART init
	uart_init();
	// SPI init
	spiInit();

#ifdef SPI_MASTER
	clearScreen();
#endif

	// main loop
	for(;;){
#ifdef SPI_MASTER
		char c = getchar();
		key(c);
#else
		printf("test1\n");
		_delay_us(1);
#endif
	}
    return 0;
}

