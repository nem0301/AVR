#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <common.h>
#include <uart.h>
#include <spi.h>

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
		int len = argc - 1;
		unsigned char tx[len];
		unsigned char rx[len];

		for (int i = 0; i < len; i++)
		{
			tx[i] = atoi(argv[i + 1]);
		}

		spiTranceive(tx, rx, len);

		printf("spi ret = %02x\n", rx[len -1]);

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

void printArr(unsigned char *arr, int len)
{
	printf("arr : ");
	for (int i = 0; i < len; i++) printf("%02x ", arr[i]);
	printf("\n");
}

void sdCommand(unsigned char cmd, unsigned long arg,
		unsigned char crc, unsigned char read, 
		unsigned char *buffer)
{
	unsigned char tx[14];
	unsigned char rx[14];
	int len = read + 6;

	tx[0] = cmd;

	for(int i = 1; i < 5; i++) tx[i] = arg >> ((4 - i) * 8);
	
	tx[5] = crc;

	for(int i = 6; i < len; i++) tx[i] = 0xff;

	spiTranceive(tx, rx, len);

	for (int i = 0; i < read; i++) buffer[i] = rx[i + 6];


#ifdef DEBUG
	for (int i = 0; i < len; i++) printf("rx %d : %02x\n", i, rx[i]);
#endif

}

#define CMD(index) (0x40 | index)
#define ACMD(index) (0x40 | ((index) - 55))
void sdInit()
{
	unsigned char rx[8] = {0, };
	spiGenClock(10);
	sdCommand(CMD(0), 0x00000000, 0x95, 8, rx); _delay_ms(1000);
	printArr(rx, sizeof(rx));
	sdCommand(CMD(8), 0x000001aa, 0x87, 8, rx); _delay_ms(1000);
	printArr(rx, sizeof(rx));
	sdCommand(ACMD(41), 0x40000000, 0xff, 8, rx); _delay_ms(1000);
	printArr(rx, sizeof(rx));
}


int main(void)
{
    // UART init
	uart_init();
	// SPI init
	spiInit();

	clearScreen();
	printf("boot\n");

	sdInit();

	// main loop
	for(;;){
		char c = getchar();
		key(c);
	}
    return 0;
}

