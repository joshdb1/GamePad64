/* keypad.c
*	author: Adam Fletcher and Joshua Brock
*/

#include "stm32l476xx.h"
#include "LCD.h"
#include "keypad.h"

//Initialize the keypad
void keypadInit(void){
	if((RCC->AHB2ENR & RCC_AHB2ENR_GPIOAEN) == 0){RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;}//Enable the clocks for GPIO A (1,2,3,5)
	if((RCC->AHB2ENR & RCC_AHB2ENR_GPIOEEN) == 0){RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;}//Enable the clocks for GPIO E (10,11,12,13)
	GPIOA->MODER &= 0xFFFFF303;																													//Configure GPIO A as input mode, pull down
		
	GPIOE->OSPEEDR &= 0xF00FFFFF;
	GPIOE->OSPEEDR |= 0x0AA00000;
	GPIOE->OTYPER |= 0x00003C00; 
	GPIOE->MODER &= 0xF00FFFFF;
	GPIOE->MODER |= 0x05500000;																													//Configure GPIO E as output, output speed high, open drain
}

//initialize joystick
void joystickInit(void){
	//enable GPIOA clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	//Configure PINS PA0,1,2,3,5 as input mode
	GPIOA->MODER &= ~((uint32_t) 0xCFF);
	//Configure PINS PA0,1,2,3,5 as pull down
	GPIOA->PUPDR &= ~((uint32_t) 0xCFF);
	GPIOA->PUPDR |= 0x8AA;
}

//get input from keypad
uint8_t keypad(void){
	uint8_t col = 0, row = 0;
	uint32_t col_address[4]={0x00000002,0x00000004,0x00000008,0x00000020};
	uint8_t keypad[4][4] = {{1,4,7,0xF},{2,5,8,0},{3,6,9,0xE},{0xA,0xB,0xC,0xD}};
	uint8_t key=100;
	
	while(key==100){
		GPIOE->ODR &= 0xFFFFC3FF;
		delay(1000);
		while((GPIOA->IDR & 0x0000002E) == 0x0000002E);
		delay(10000);
		row=0;
		while(row<=3){	
			GPIOE->ODR &= 0xFFFFC3FF;
			switch(row){																																			//	change column high
				case 0: 
								GPIOE->ODR |= 0x00003800;
								break;
				case 1:
								GPIOE->ODR |= 0x00003400;
								break;
				case 2:
								GPIOE->ODR |= 0x00002C00;
								break;
				case 3:
								GPIOE->ODR |= 0x00001C00;
								break;
				default: row=0;
			}
			delay(1000);																																		//	delay
			for(col = 0;col<4;col++){																													//	check if rows high,
				if((GPIOA->IDR & col_address[col])==0){
					key =keypad[col][row];
				}
			}
			row++;
			while((GPIOA->IDR & 0x0000002E) != 0x0000002E);
		}
	}
	return key;
}
//End

//void delay(int delayPeriod){
//  int volatile j;
//	for(int i = 0; i < delayPeriod; i++){
//    j = i;
//  }
//}


