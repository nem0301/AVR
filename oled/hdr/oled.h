#ifndef __OLED_h__
#define __OLED_h__

#include <avr/io.h>
#include <util/delay.h>

#define OLED_DDR 	DDRB
#define OLED_PORT	PORTB

#define OLED_SCKL	PORTB5
#define OLED_SDIN	PORTB3
#define OLED_CS		PORTB2
#define OLED_DC		PORTB1 	//high : data, low : command
#define OLED_RES	PORTB0


void oledCommand(unsigned char cmd)
{
	OLED_PORT &= ~_BV(OLED_DC);

	SPDR = cmd;
	while (!(SPSR & _BV(SPIF)));
}

void oledData(unsigned char data)
{
	OLED_PORT |= _BV(OLED_DC);

	SPDR = data;
	while (!(SPSR & _BV(SPIF)));

}

void oledInit()
{
	OLED_PORT &= ~_BV(OLED_CS);
	OLED_DDR |= _BV(OLED_CS) | _BV(OLED_SCKL) | _BV(OLED_SDIN) |
		_BV(OLED_DC) | _BV(OLED_RES);
	SPCR = (_BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA));
	SPSR |= _BV(SPI2X); // SCLK 8MHz

	// power on
	OLED_PORT &= ~_BV(OLED_RES);
	_delay_us(2);
	OLED_PORT |= _BV(OLED_RES);
	_delay_us(2);
	OLED_PORT &= ~_BV(OLED_DC);

	oledCommand(0xaf);
	_delay_ms(300);

	for (int i = 0; i < 128; i++)
	{
		oledCommand(0xa1);
		oledData(i);
		_delay_ms(1);
	}
}


#endif
