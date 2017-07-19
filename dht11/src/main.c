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
#include <dht11.h>

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
    if (strcmp(cmd, "clear") == 0) {
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

uint8_t DHT11_GetData(uint8_t *data1, uint8_t *data2) 
{
	uint8_t bits1[5];
	uint8_t bits2[5];
	uint8_t i, j = 0;

	//reset port
	DHT11_DDR |= _BV(DHT11_INPUTPIN1) | _BV(DHT11_INPUTPIN2); //output
	DHT11_PORT |= _BV(DHT11_INPUTPIN1) | _BV(DHT11_INPUTPIN2); //high
	_delay_ms(100);

	//send request
	DHT11_PORT &= ~_BV(DHT11_INPUTPIN1); //low
	_delay_ms(18);
	DHT11_PORT |= _BV(DHT11_INPUTPIN1); //high
	_delay_us(1);
	DHT11_DDR &= ~_BV(DHT11_INPUTPIN1); //input
	_delay_us(39);

	//check start condition 1
	if ((DHT11_PIN & _BV(DHT11_INPUTPIN1))) {
		return 1;
	}
	_delay_us(80);
	//check start condition 2
	if (!(DHT11_PIN & _BV(DHT11_INPUTPIN1))) {
		return 2;
	}
	_delay_us(80);

	//read the data
	for (j = 0; j < 5; j++) { //read 5 byte
		uint8_t result = 0;
		for (i = 0; i < 8; i++) { //read every bit
			while (!(DHT11_PIN & _BV(DHT11_INPUTPIN1)))
				; //wait for an high input
			_delay_us(30);
			if (DHT11_PIN & _BV(DHT11_INPUTPIN1)) //if input is high after 30 us, get result
				result |= (1 << (7 - i));
			while (DHT11_PIN & _BV(DHT11_INPUTPIN1))
				; //wait until input get low
		}
		bits1[j] = result;

	}

	//send request
	DHT11_PORT &= ~_BV(DHT11_INPUTPIN2); //low
	_delay_ms(18);
	DHT11_PORT |= _BV(DHT11_INPUTPIN2); //high
	_delay_us(1);
	DHT11_DDR &= ~_BV(DHT11_INPUTPIN2); //input
	_delay_us(39);

	//check start condition 1
	if ((DHT11_PIN & _BV(DHT11_INPUTPIN2))) {
		return 1;
	}
	_delay_us(80);
	//check start condition 2
	if (!(DHT11_PIN & _BV(DHT11_INPUTPIN2))) {
		return 2;
	}
	_delay_us(80);
	for (j = 0; j < 5; j++) { //read 5 byte
		uint8_t result = 0;
		for (i = 0; i < 8; i++) { //read every bit
			while (!(DHT11_PIN & _BV(DHT11_INPUTPIN2)))
				; //wait for an high input
			_delay_us(30);
			if (DHT11_PIN & _BV(DHT11_INPUTPIN2)) //if input is high after 30 us, get result
				result |= (1 << (7 - i));
			while (DHT11_PIN & _BV(DHT11_INPUTPIN2))
				; //wait until input get low
		}
		bits2[j] = result;
	}

	//reset port
	DHT11_DDR |= _BV(DHT11_INPUTPIN1) | _BV(DHT11_INPUTPIN2); //output
	DHT11_PORT |= _BV(DHT11_INPUTPIN1) | _BV(DHT11_INPUTPIN2); //low
	_delay_ms(100);

	//check checksum
	if (bits1[0] + bits1[1] + bits1[2] + bits1[3] == bits1[4] &&
		bits2[0] + bits2[1] + bits2[2] + bits2[3] == bits2[4]) 
	{
		data1[0] = bits1[2];
		data1[1] = bits1[0];
		data2[0] = bits2[2];
		data2[1] = bits2[0];
		return 0;
	}

	return 3;
}

int main(void)
{
    // UART init
	uart_init();
	// SPI init
	spiInit();

	clearScreen();
	uint8_t data1[2];
	uint8_t data2[2];

	uint8_t err;
	for (;;)
	{
		if ((err = DHT11_GetData(data1, data2)) == 0)
		{
			printf("data1\n\ttemp %03d, humi : %03d\n", data1[0], data1[1]);
			printf("data2\n\ttemp %03d, humi : %03d\n", data2[0], data2[1]);
		}
		else
		{
			printf("err = %d\n", err);
		}
		_delay_ms(1000);
	}
	
	// main loop
	for(;;){
		char c = getchar();
		key(c);
	}
    return 0;
} 
