#ifndef __SPI_H__
#define __SPI_H__

#include <avr/io.h>

#define SPI_DDR     DDRB
#define SPI_PORT    PORTB

#define SCK         PORTB5
#define MISO        PORTB4
#define MOSI        PORTB3
#define SS          PORTB2


void spiInit();
void spiTranceive(unsigned char *tx, unsigned char *rx, int len);

#endif
