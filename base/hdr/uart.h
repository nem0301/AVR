#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>

void uart_init(void);
int uart_putchar(char c, FILE *stream);
int uart_getchar(FILE *stream);

#endif

