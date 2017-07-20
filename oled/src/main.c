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
#include <oled.h>

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
		int len = 1;
		unsigned char tx[len];
		unsigned char rx[len];
		if (argc == 2)
		{
			tx[0] = atoi(argv[3]);
			spiTranceive(tx, rx, len);
		}
		else
		{
			printf("format err : spi [data]\n");
		}
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

int main(void)
{
    // UART init
	uart_init();

	oledInit();

	clearScreen();

	printf("%s\n", prompt);

	// main loop
	for(;;){
		char c = getchar();
		key(c);
	}
    return 0;
}

