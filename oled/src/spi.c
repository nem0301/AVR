#include <stdio.h>
#include <util/delay.h>

#include <spi.h>

void spiInit()
{
		SPI_PORT |= _BV(SS);
		SPI_DDR |= (_BV(SS) | _BV(SCK) | _BV(MOSI));
		SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR0)); // 16M/16=1Mhz
}

void spiTranceive(unsigned char *tx, unsigned char *rx, int len)
{
	SPI_PORT &= ~_BV(SS);

	for (int i = 0; i < len; i++)
	{
		_delay_us(100);
		SPDR = *(tx + i);
		while (!(SPSR & _BV(SPIF)));
		if (rx != NULL)
			*(rx + i) = SPDR;
	}

	SPI_PORT |= _BV(SS);
}

