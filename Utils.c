/**
 * @file Utils.c
 * @author Joshua Brock (joshdb1@gmail.com)
 * @brief Contains any miscellaneous utilities we need for driving the games
 * @version 0.1
 * @date 2020-12-15
 * 
 * 
 */

#include "Utils.h"

/**
 * @brief delays the specified number of clock cycles
 * 
 * @param delay the number of clock cycles to delay
 */
void delay(volatile uint32_t delay)
{
	volatile uint32_t j;
	for (volatile uint32_t i = 0; i < delay; ++i)
	{
		j = i;
	}
}
