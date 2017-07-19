#ifndef __DHT_H__
#define __DHT_H__


//setup parameters
#define DHT11_DDR DDRD
#define DHT11_PORT PORTD
#define DHT11_PIN PIND
#define DHT11_INPUTPIN1 PD2
#define DHT11_INPUTPIN2 PD3

#define DHT11_DATA_TEMPERATURE 0
#define DHT11_DATA_HUMIDITY 1
#define DHT11_ERROR 150

uint8_t DHT11_GetData(uint8_t *data1, uint8_t *data2);

#endif
