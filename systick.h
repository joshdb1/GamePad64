#ifndef SYSTICK_H
#define SYSTICK_H

#include "stm32l476xx.h"

void SysTick_Initialize(uint32_t ticks);
void SysTick_Handler(void);

#endif
