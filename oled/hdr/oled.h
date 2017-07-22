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

#define OLED_BLACK	0, 0, 0
#define OLED_WHITE	127, 127, 127

#define OLED_RED	127, 0, 0
#define OLED_GREEN	0, 127, 0
#define OLED_BLUE	0, 0, 127

#define OLED_YELLOW	127, 127, 0
#define OLED_PURPLE	127, 0, 127
#define OLED_CYAN	0, 127, 127


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

	// setting rgb
	uint8_t data = 0b10100000;
	
	oledCommand(0xa0);
	oledData(data);
}


void scrollV(unsigned char n)
{
	oledCommand(0xa1);
	oledData(n);
}

// 0 : all off, 1 : all on, 2 : reset normal display, 3 : inverse
void displayMode(unsigned char mode)
{
	if ( mode > 3 ) return;
	oledCommand(0xa4 | mode);
}

// 0 : sleep, 1 : wake
void sleepMode(unsigned char mode)
{
	if ( mode > 1 ) return;
	oledCommand(0xaE | mode);
}

// if value NULL then reset GS, else set value
void grayScalePurseWidth(unsigned char *value)
{
	if ( value == NULL)
	{
		oledCommand(0xb9);
		return;
	}
		
	oledCommand(0xb8);
	for (int i = 0; i < 63; i++)
	{
		if (value[i] > 180) 
		{
			oledData(180);
			continue;
		}
		oledData(value[i]);
	}
}

void contrast(unsigned char value)
{
	if ( value > 15 ) return;
	oledCommand(0xc7);
	oledData(value);
}

void cursor(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
	oledCommand(0x15);	// set col
	oledData(x);
	oledData(x + w - 1);
	
	oledCommand(0x75);	// set row
	oledData(y);
	oledData(y + h - 1);
}

void drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		uint8_t r, uint8_t g, uint8_t b)
{
	cursor(x, y, w, h);

	oledCommand(0x5c);
	for (int x = 0; x < w + 1; x++)
	{
		for (int y = 0; y < h + 1; y++)
		{
			oledData(b);
			oledData(g);
			oledData(r);
		}
	}
}



void oledTest()
{
//	for (int i = 0; i < 128; i++)
//	{
//		scrollV(i);
//		_delay_ms(3);
//	}
//	_delay_ms(1000);

//
//	for (int i = 0; i < 4; i++)
//	{
//		displayMode(i);
//		_delay_ms(1000);
//	}

//	for (int i = 0; i < 2; i++)
//	{
//		sleepMode(i);
//		_delay_ms(1000);
//	}


//	unsigned char data[63];
//	for (int i = 0; i < 63; i++)
//	{
//		data[i] = i * 2;
//	}
//	grayScalePurseWidth(data);
//
//	_delay_ms(1000);
//
//	grayScalePurseWidth(NULL);
//
//	_delay_ms(1000);
//

	scrollV(32);

	drawRect(0, 0, 128, 128, OLED_RED);
	_delay_ms(500);
	drawRect(0, 0, 128, 128, OLED_WHITE);
	_delay_ms(500);
	uint8_t k = 0;
	uint8_t m = 0;
	uint8_t l = 0;
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if ( i < 4 )
			{
				drawRect(j * 8, i * 10, 8, 10,  k++, 0, 0);
			}
			else if ( i >= 4 && i < 8)
			{
				drawRect(j * 8, i * 10, 8, 10,  0, m++, 0);
			}
			else
			{
				drawRect(j * 8, i * 10, 8, 10,  0, 0, l++);
			}
		}
	}


	_delay_ms(1000);

	oledCommand(0xb2);
	oledData(0xa4);
	oledData(0x00);
	oledData(0x00);

	_delay_ms(3000);

	drawRect(0, 0, 128, 128, OLED_RED);
	_delay_ms(500);
	drawRect(0, 0, 128, 128, OLED_WHITE);
	_delay_ms(500);
	k = 0;
	m = 0;
	l = 0;
	for (int i = 0; i < 12; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			if ( i < 4 )
			{
				drawRect(j * 8, i * 10, 8, 10,  k++, 0, 0);
			}
			else if ( i >= 4 && i < 8)
			{
				drawRect(j * 8, i * 10, 8, 10,  0, m++, 0);
			}
			else
			{
				drawRect(j * 8, i * 10, 8, 10,  0, 0, l++);
			}
		}
	}
}




#endif
