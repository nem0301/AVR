#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#define BAUD 57600
#include <util/setbaud.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// SPI
#define MASTER
#define SPI_DDR     DDRB
#define SPI_PORT    PORTB

#define SCK         PORTB5
#define MISO        PORTB4
#define MOSI        PORTB3
#define SS          PORTB2

#define CMD_NUM     6
#define CMDSIZE     50
#define HISTSIZE    20


void uart_init(void);
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

void SPI_init();
unsigned char masterWrite(unsigned char data);
unsigned char slaveRead(unsigned char prev);

void clearScreen();
void setCursor(int x, int y);
void specialKey(char c);
void func(char *cmd);





#endif // COMMON_H_INCLUDED
