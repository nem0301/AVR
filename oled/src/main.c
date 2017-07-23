#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/setbaud.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <oled.h>
#include <dht11.h>

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
	_delay_ms(1000);

	oledInit();

	uint8_t data1[2];
	uint8_t data2[2];

	uint8_t err;
	for (;;)
	{
		color_t color = {0, 127, 0};
		if ((err = DHT11_GetData(data1, data2)) == 0)
		{
			char tmp[17];
			sprintf(tmp, "t : %03d  h : %03d", data1[0], data1[1]);
			drawString(0, 0, color, tmp);
			sprintf(tmp, "t : %03d  h : %03d", data2[0], data2[1]);
			drawString(0, 11, color, tmp);
		}
		else
		{
			drawRect(0, 0, 128, 22, OLED_BLACK);
			drawString(0, 11, color, "err");
		}
		_delay_ms(1000);
	}

    return 0;
}

