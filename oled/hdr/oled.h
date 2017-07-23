#ifndef __OLED_h__
#define __OLED_h__
#include <string.h>
#include <stdio.h>

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


void scrollV(uint8_t n);
void genChar();

void oledCommand(uint8_t cmd)
{
	OLED_PORT &= ~_BV(OLED_DC);

	SPDR = cmd;
	while (!(SPSR & _BV(SPIF)));
}

void oledData(uint8_t data)
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
	uint8_t data = 0b10110000;
	
	oledCommand(0xa0);
	oledData(data);

	scrollV(32);

	// enhanced display
	oledCommand(0xb2);
	oledData(0xa4);
	oledData(0x00);
	oledData(0x00);

	drawRect(0, 0, 128, 128, OLED_WHITE);
	_delay_ms(500);
	drawRect(0, 0, 128, 128, OLED_BLACK);
	_delay_ms(500);
}


void scrollV(uint8_t n)
{
	oledCommand(0xa1);
	oledData(n);
}

// dir -1 : right, 0 : stop, 1 : left
// start : start row
// num : number of rows
// spped 0 : fast, 1 : normal, 2 : slow, 3 : slowest
void scrollH(uint8_t dir, uint8_t start, uint8_t num, uint8_t speed)
{
	oledCommand(0x96);
	oledData(dir);
	oledData(start);
	oledData(num);
	oledData(0);
	oledData(speed);
}

void startScrollH()
{
	oledCommand(0x9f);
}

void stopScrollH()
{
	oledCommand(0x9e);
}


// 0 : all off, 1 : all on, 2 : reset normal display, 3 : inverse
void displayMode(uint8_t mode)
{
	if ( mode > 3 ) return;
	oledCommand(0xa4 | mode);
}

// 0 : sleep, 1 : wake
void sleepMode(uint8_t mode)
{
	if ( mode > 1 ) return;
	oledCommand(0xaE | mode);
}

// if value NULL then reset GS, else set value
void grayScalePurseWidth(uint8_t *value)
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

void colorContrast(uint8_t r, uint8_t g, uint8_t b)
{
	oledCommand(0xc1);
	oledData(b);
	oledData(g);
	oledData(r);
}

void contrast(uint8_t value)
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
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			oledData(b);
			oledData(g);
			oledData(r);
		}
	}
}

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

void drawImage(uint8_t x, uint8_t y, uint8_t w, uint8_t h,
		color_t *data)
{
	cursor(x, y, w, h);

	oledCommand(0x5c);
	int i = 0;
	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++)
		{
			oledData(data[i].b);
			oledData(data[i].g);
			oledData(data[i].r);
			i++;
		}
	}
}


//uint8_t map[11 * 128];
const uint8_t map[1408] =
{
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,          //
	12,30,30,30,12,12,0,12,12,0,0,  // !
	54,54,20,0,0,0,0,0,0,0,0,       // "
	0,18,18,63,18,18,63,18,18,0,0,  // #
	12,30,51,3,30,48,51,30,12,12,0, // $
	0,0,35,51,24,12,6,51,49,0,0,    // %
	12,30,30,12,54,27,27,27,54,0,0, // &
	12,12,6,0,0,0,0,0,0,0,0,        // '
	24,12,6,6,6,6,6,12,24,0,0,      // (
	6,12,24,24,24,24,24,12,6,0,0,   // )
	0,0,0,51,30,63,30,51,0,0,0,     // *
	0,0,0,12,12,63,12,12,0,0,0,     // +
	0,0,0,0,0,0,0,12,12,6,0,        // ,
	0,0,0,0,0,63,0,0,0,0,0,         // -
	0,0,0,0,0,0,0,12,12,0,0,        // .    
	0,0,32,48,24,12,6,3,1,0,0,      // /

	12,30,51,51,51,51,51,30,12,0,0, // 0
	12,14,15,12,12,12,12,12,63,0,0, // 1
	30,51,48,24,12,6,3,51,63,0,0,   // 2
	30,51,48,48,28,48,48,51,30,0,0, // 3
	16,24,28,26,25,63,24,24,60,0,0, // 4
	63,3,3,31,48,48,48,51,30,0,0,   // 5
	28,6,3,3,31,51,51,51,30,0,0,    // 6
	63,49,48,48,24,12,12,12,12,0,0, // 7
	30,51,51,51,30,51,51,51,30,0,0, // 8
	30,51,51,51,62,48,48,24,14,0,0, // 9

	0,0,12,12,0,0,12,12,0,0,0,      // :
	0,0,12,12,0,0,12,12,6,0,0,      // ;
	0,0,24,12,6,3,6,12,24,0,0,      // <
	0,0,0,63,0,0,63,0,0,0,0,        // =
	0,0,3,6,12,24,12,6,3,0,0,       // >
	30,51,51,59,59,59,27,3,30,0,0,  // @
	30,51,51,24,12,12,0,12,12,0,0,  // ?

	12,30,51,51,63,51,51,51,51,0,0,          // A ** TO BE FILLED **
	31,51,51,51,31,51,51,51,31,0,0, // B
	28,54,35,3,3,3,35,54,28,0,0,    // C
	15,27,51,51,51,51,51,27,15,0,0, // D
	63,51,35,11,15,11,35,51,63,0,0, // E
	63,51,35,11,15,11,3,3,3,0,0,    // F
	28,54,35,3,59,51,51,54,44,0,0,  // G
	51,51,51,51,63,51,51,51,51,0,0, // H
	30,12,12,12,12,12,12,12,30,0,0, // I
	60,24,24,24,24,24,27,27,14,0,0, // J
	51,51,51,27,15,27,51,51,51,0,0, // K
	3,3,3,3,3,3,35,51,63,0,0,       // L
	33,51,63,63,51,51,51,51,51,0,0, // M
	51,51,55,55,63,59,59,51,51,0,0, // N
	30,51,51,51,51,51,51,51,30,0,0, // O
	31,51,51,51,31,3,3,3,3,0,0,     // P
	30,51,51,51,51,51,63,59,30,48,0,// Q
	31,51,51,51,31,27,51,51,51,0,0, // R
	30,51,51,6,28,48,51,51,30,0,0,  // S
	63,63,45,12,12,12,12,12,30,0,0, // T
	51,51,51,51,51,51,51,51,30,0,0, // U
	51,51,51,51,51,30,30,12,12,0,0, // V
	51,51,51,51,51,63,63,63,18,0,0, // W
	51,51,30,30,12,30,30,51,51,0,0, // X
	51,51,51,51,30,12,12,12,30,0,0, // Y
	63,51,49,24,12,6,35,51,63,0,0,  // Z

	30,6,6,6,6,6,6,6,30,0,0,          // [
	0,0,1,3,6,12,24,48,32,0,0,        // "\"
	30,24,24,24,24,24,24,24,30,0,0,   // ]
	8,28,54,0,0,0,0,0,0,0,0,          // ^
	0,0,0,0,0,0,0,0,0,63,0,           // _
	6,12,24,0,0,0,0,0,0,0,0,          // `

	0,0,0,14,24,30,27,27,54,0,0,      // a
	3,3,3,15,27,51,51,51,30,0,0,      // b
	0,0,0,30,51,3,3,51,30,0,0,        // c
	48,48,48,60,54,51,51,51,30,0,0,  // d
	0,0,0,30,51,63,3,51,30,0,0,      // e
	28,54,38,6,15,6,6,6,15,0,0,      // f
	0,0,30,51,51,51,62,48,51,30,0,   // g
	3,3,3,27,55,51,51,51,51,0,0,     // h
	12,12,0,14,12,12,12,12,30,0,0,   // i
	48,48,0,56,48,48,48,48,51,30,0,  // j
	3,3,3,51,27,15,15,27,51,0,0,     // k
	14,12,12,12,12,12,12,12,30,0,0,  // l
	0,0,0,29,63,43,43,43,43,0,0,     // m
	0,0,0,29,51,51,51,51,51,0,0,     // n
	0,0,0,30,51,51,51,51,30,0,0,     // o
	0,0,0,30,51,51,51,31,3,3,0,      // p
	0,0,0,30,51,51,51,62,48,48,0,    // q
	0,0,0,29,55,51,3,3,7,0,0,        // r
	0,0,0,30,51,6,24,51,30,0,0,      // s
	4,6,6,15,6,6,6,54,28,0,0,        // t
	0,0,0,27,27,27,27,27,54,0,0,     // u
	0,0,0,51,51,51,51,30,12,0,0,     // v
	0,0,0,51,51,51,63,63,18,0,0,     // w
	0,0,0,51,30,12,12,30,51,0,0,     // x
	0,0,0,51,51,51,62,48,24,15,0,    // y
	0,0,0,63,27,12,6,51,63,0,0,      // z

	56,12,12,12,7,12,12,12,56,0,0,   // {
	12,12,12,12,12,12,12,12,12,0,0,  // |
	7,12,12,12,56,12,12,12,7,0,0,    // }
	38,45,25,0,0,0,0,0,0,0,0,        // ~
};

void drawChar(uint8_t x, uint8_t y, color_t color, char c)
{
	cursor(x, y, 8, 11);
	oledCommand(0x5c);
	int i = c * 11;
	for (int y = 0; y < 11; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if ( (map[i] & _BV(x)) != 0)
			{
				oledData(color.b);
				oledData(color.g);
				oledData(color.r);
			}
			else
			{
				oledData(0);
				oledData(0);
				oledData(0);
			}
		}
		i++;
	}
}

void drawString(uint8_t x, uint8_t y, color_t color, const char* str)
{
	for (uint8_t i = 0; i < strlen(str); i++)
	{
		drawChar(x + (i * 8), y, color, str[i]);
	}
}

void screenTest()
{
	drawRect(0, 0, 128, 128, OLED_RED);
	_delay_ms(500);
	drawRect(0, 0, 128, 128, OLED_WHITE);
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
	_delay_ms(500);
}


#endif
