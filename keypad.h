/* keypad.h
*	author: Adam Fletcher
*/

#ifndef KEYPAD_H
#define KEYPAD_H

#include "stm32l476xx.h"
#include "LCD.h"

void keypadInit(void);
void joystickInit(void);
void delay(int delayPeriod);
uint8_t keypad(void);

#endif
