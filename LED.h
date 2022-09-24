/**
 * @file LED.h
 * @author Joshua Brock (joshdb1@gmail.com), Adam Fletcher (adam.fletcher@aggiemail.usu.edu)
 * @brief 
 * @version 0.1
 * @date 2020-12-14
 * 
 * 
 */
#ifndef LED_H
#define LED_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief how many LEDs to drive. Can be an arbitrary number. 
 *        Redefine for different sized LED arrays or strips
 * 
 */
#define LED_COUNT 64


#define LED_BOARD_WIDTH 8
#define LED_BOARD_HEIGHT 8


/**
 * @brief definitions for the color codes we use. 
 *        Add more definitions if you'd like.
 *         
 * Note: Colors are represented as an int with the last 24-bits
 *       being the color data. Color data is ordered green, red, blue 
 *       going from most significant bit to least significant bit,
 *       8-bits for each.
 *              
 * 
 */
enum ColorCodes
{
  //COLOR_RESET = 0x7fffffff,   // not usually used as an acual color
	Bl = 0,
  W = 0x666666,//0x0c0c0c,               // white
  G = 0xaa0000,//0x0c0000,               // green
  R = 0x00aa00,//0x000c00,               // red
  B = 0x0000aa,//0x00000c,               // blue
	SB = 0x690094//0x050007								// sea blue
};

// #ifndef COLOR_RESET
#define COLOR_RESET 0xFFFFFFFF
// #endif

void LED_Init(void);
void updateLED(uint32_t rgbData[LED_COUNT], bool blinking);
void updateLEDBlink(uint32_t data1[LED_COUNT], uint32_t data2[LED_COUNT]);
void clearLED(void);
void testLEDs(void);
void testBlink(void);

#endif
