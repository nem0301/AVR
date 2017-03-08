#include "common.h"

void SPI_init()
{
#ifdef MASTER
    SPI_PORT |= _BV(SS1) | _BV(SS2);
    SPI_DDR |= _BV(SS2) | (_BV(SS1) | _BV(SCK) | _BV(MOSI));
    SPCR = (_BV(SPE) | _BV(MSTR) | _BV(SPR0)); // 16M/16=1Mhz
    SPCR |= (_BV(CPOL) | _BV(CPHA));
#else
    SPI_DDR |= _BV(MISO);
    SPCR = _BV(SPE);
#endif // MASTER
}

unsigned char spiTranceive(unsigned char ss,
                           unsigned char *rxBuffer,
                           unsigned char *txBuffer,
                           int len)
{
    int i;

    if (ss == 1)
    {
        SPI_PORT &= ~_BV(SS1);
    }
    else if (ss == 2)
    {
        SPI_PORT &= ~_BV(SS2);
    }

    for (i = 0; i < len; i++)
    {
        unsigned char data = txBuffer[i];
        SPDR = data;
        while (!(SPSR & _BV(SPIF)));
        rxBuffer[i] = (unsigned char)SPDR;
    }

    if (ss == 1)
    {
        SPI_PORT |= _BV(SS1);
    }
    else if (ss == 2)
    {
        SPI_PORT |= _BV(SS2);
    }

    return 0;
}

unsigned char masterWrite(unsigned char data)
{
    SPI_PORT &= ~_BV(SS1);
    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    return SPDR;
}

unsigned char slaveRead(unsigned char prev)
{
    unsigned char cmd = 0;
    int addr = 0;
    uint8_t data = 0xff;

    SPDR = data;
    while (!(SPSR & _BV(SPIF)));
    cmd = SPDR;

    addr = 0x7f & cmd;
    data = (uint8_t)eeprom_read_byte((uint8_t*)(addr));
    SPDR = data;

    printf("cmd : %02x addr : %04x data : %02x\n", cmd, addr, data);
    while (!(SPSR & _BV(SPIF)));

    // read
    if (cmd & 0x80)
    {
        data = SPDR;
        printf("read\n");
    }
    // write
    else
    {
        printf("write\n");
        data = SPDR;
        eeprom_write_byte((uint8_t*)addr, data);
    }

    return data;
}

