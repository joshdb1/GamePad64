/* uart.h
*	author: Adam Fletcher
*/

#ifndef UART_H
#define UART_H

#define MIN 1920000000
#define SEC 32000000

#include "stm32l476xx.h"
#include "LCD.h"
#include <stdio.h>

void uartInit(void);
void USART_Init(USART_TypeDef *USARTx);
uint8_t uartConnect(void);
void uartSend(uint8_t message);
uint8_t uartReceive(uint32_t delay);

#endif
