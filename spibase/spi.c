#include "common.h"

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
