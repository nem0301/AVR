#include <stdio.h>
#include <util/delay.h>

#ifndef SPI_MASTER
#include <avr/interrupt.h>
#endif

#include <spi.h>

// ms 1 : master, 0 : slave
void spiInit()
{
#ifdef SPI_MASTER
		SPI_PORT |= _BV(SS);
		SPI_DDR |= (_BV(SS) | _BV(SCK) | _BV(MOSI));
		SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR0)); // 16M/16=1Mhz
#else
		SPI_DDR |= _BV(MISO);
		SPCR = _BV(SPE) | _BV(SPIE);
		sei();
#endif
}

#ifdef SPI_MASTER

void spiTranceive(unsigned char inst, unsigned char op, unsigned char *data)
{
	SPI_PORT &= ~_BV(SS);
	SPDR = 0xff;
	while (!(SPSR & _BV(SPIF)));

	_delay_ms(1);
	SPDR = inst;
	while (!(SPSR & _BV(SPIF)));

	_delay_ms(1);
	SPDR = op;
	while (!(SPSR & _BV(SPIF)));

	_delay_ms(1);
	SPDR = *data;
	while (!(SPSR & _BV(SPIF)));
	*data = SPDR;

	SPI_PORT |= _BV(SS);
}


#endif
