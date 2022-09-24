/**
 * @file LED.c
 * @author Joshua Brock (joshdb1@gmail.com), Adam Fletcher (adam.fletcher@aggiemail.usu.edu)
 * @brief Contains all functions necessary for driving a ws2812 LED matrix from an STM32L476 Discovery Board.
 * 				Should support an arbitrarly sized LED matrix or strip. Update macros accordingly.
 * @version 0.1
 * @date 2020-12-14
 * 
 * 
 */

#include "stm32l476xx.h"
#include "LED.h"
#include "Utils.h"
#include <stdlib.h>

/**
 * @begin macros
 * 
 */
#define BITS_PER_PIXEL 24			// typically 24 bits per pixel (one uint8_t for each value in R, G, and B)
#define TIMER_PSC 0
#define TIMER_ARR 99
#define DUTY_0 30
#define DUTY_1 70	
#define DUTY_RESET 0					// This should always be 0

#define DUTY_RESET_CYCLES 10 	// How many cycles to reset with by default (duty ratio 0)
#define DUTY_LEN (LED_COUNT * BITS_PER_PIXEL + DUTY_RESET_CYCLES) // 24 bits for each LED buffered
/**
 * @end macros
 * 
 */

/**
 * @begin global variables
 * 
 */
static volatile uint8_t dutyData[DUTY_LEN]; // contains the duty ratios to use for timer 1

static volatile bool blink1 = true;
static uint32_t rgbBlink1[LED_COUNT];
static uint32_t rgbBlink2[LED_COUNT];

static volatile uint32_t counter = 0;				// used to index into dutyData
/**
 * @end global variables
 * 
 */


/**
 * @begin forward declarations (only private functions)
 * 
 */
void Clock_Init(void);
void PE8_Init(void);
void TIM1_Init(void);
void TIM1_UP_TIM16_IRQHandler(void);
void SysTick_Init(uint32_t ticks);
void SysTick_Handler(void);
void fillDuty(uint32_t rgbData[LED_COUNT]);
/**
 * @end forward declarations
 * 
 */

///////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////

/**
 * @brief initialize all peripherals needed to drive the LED matrix
 * 
 */
void LED_Init(void)
{
	// Initialize the PLL and PCLK2 prescalar
	Clock_Init();
	
	// Initialize systick which drivers the LED blinking
	SysTick_Init(16777215);

	// Initialize PE8 as Timer1 output
	PE8_Init();

	//	Initialize Timer 1
	TIM1_Init();
}

/**
 * @brief updates the LED matrix from the input data array
 * 
 * @param rgbData the raw RGB data to use for the LEDs
 */
void updateLED(uint32_t rgbData[LED_COUNT], bool blinking)
{
	counter = 0;
	if (!blinking) SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

	// fill the duty ratio buffer from the raw RGB data
	fillDuty(rgbData);

	TIM1->CR1 |= TIM_CR1_CEN;

	//update board just once
	while (counter < DUTY_LEN)
		;

	//turn off the timer
	TIM1->CR1 &= ~TIM_CR1_CEN;
}

/**
 * @brief switches between the two data arrays at a fequency of 
 * 				about 4Hz
 * 
 * @param data1 
 * @param data2 
 */
void updateLEDBlink(uint32_t data1[LED_COUNT], uint32_t data2[LED_COUNT])
{
	//updateLED(data2, false);
	for (int i = 0; i < LED_COUNT; ++i)
	{
		rgbBlink1[i] = data1[i];
		rgbBlink2[i] = data2[i];
	}
	
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/**
 * @brief clear the LEDs
 * 
 */
void clearLED(void)
{
	uint32_t blank[LED_COUNT];
	
	for (int i = 0; i < LED_COUNT; ++i)
	{
		blank[i] = 0;
	}
	
	updateLED(blank, false);
}

/**
 * @brief tests the leds
 * 
 */
void testLEDs(void)
{
	uint32_t rgbData[LED_COUNT];

	for (int i = 0; i < 64; i++)
	{
		if (i % 3 == 0)
			rgbData[i] = 0x050000;
		else if (i % 3 == 1)
			rgbData[i] = 0x000500;
		else
			rgbData[i] = 0x000005;
	}

	LED_Init();
	updateLED(rgbData, false);
}

void testBlink(void)
{
	uint32_t rgbBlink11[LED_COUNT];
	uint32_t rgbBlink22[LED_COUNT];
	for (int i = 0; i < 64; i++)
	{
		if (i % 3 == 0)
			rgbBlink11[i] = 0x050500;
		else if (i % 3 == 1)
			rgbBlink11[i] = 0xf0050f;
		else
			rgbBlink11[i] = 0x123456;
		
		if (i % 3 == 0)
			rgbBlink22[i] = 0xffffff;
		else if (i % 3 == 1)
			rgbBlink22[i] = 0xffffff;
		else
			rgbBlink22[i] = 0xffffff;
	}
	
	LED_Init();
	updateLEDBlink(rgbBlink11, rgbBlink22);
}

///////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////

/**
 * @brief initialize the clock (PLL @ 80MHz)
 * 
 */
void Clock_Init(void)
{
	uint32_t HSITrim;

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0)
		;

	HSITrim = 16;
	RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
	RCC->ICSCR |= HSITrim << 24;

	RCC->CR &= ~RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == RCC_CR_PLLRDY)
		;

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | (20U << 8);
	RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | (1u << 4);

	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
	RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) == 0)
		;

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
		;
}

/**
 * @brief initialize PE8 as output for TIM1
 * 
 */
void PE8_Init(void)
{
	//Enable clock to GPIOE
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;

	// very high speed (11)
	GPIOE->OSPEEDR &= ~((uint32_t)0x3 << (2 * 8));
	GPIOE->OSPEEDR |= (0x3 << (2 * 8));

	// alternate function choice(tim1_ch4) AF1
	GPIOE->AFR[1] &= ~((uint32_t)0xF);
	GPIOE->AFR[1] |= (0x1);

	// alternate function mode
	GPIOE->MODER &= ~((uint32_t)0x3 << (2 * 8));
	GPIOE->MODER |= (0x2 << (2 * 8));

	// push pull (0)
	GPIOE->OTYPER &= ~((uint32_t)0x01 << 8);

	// PUPDR as no pull up pull down
	GPIOE->PUPDR &= ~((uint32_t)0x3 << (2 * 8));
}

/**
 * @brief initialize timer one, which will be the PWM driver for the LED
 * 
 */
void TIM1_Init(void)
{
	//Enable Timer 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	// Counting direction: 0 = Up-counting, 1 = Downcounting
	TIM1->CR1 &= ~TIM_CR1_DIR; // Select up-counting

	//Prescaler //20MHz/(499+1)=40kHz
	TIM1->PSC = TIMER_PSC;

	//Auto-reload //PWM frequency = 40kHz/(99+1) = 400Hz
	TIM1->ARR = TIMER_ARR;

	//Clear output compare mode bits for channel 1
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;

	// Select PWM mode 2 output on channel 1 (0C1M=111)/ PWM mode 1(OC1M=110)
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // | TIM_CCMR1_OC1M_0;

	//output 1 preload enable
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;

	//select output polarity: 0=Active High, 1 = Active Low
	TIM1->CCER &= ~TIM_CCER_CC1NP; //OC1N = OCREF + CC1NP

	//Enable complementary output of channel 1 (CH1N)
	TIM1->CCER |= TIM_CCER_CC1NE;

	//Main output enable (MOE): 0 = Disable, 1 = Enable
	TIM1->BDTR |= TIM_BDTR_MOE;

	//Output compare register for channel 1
	TIM1->CCR1 = DUTY_RESET; // duty cycle = .6

	//testing
	TIM1->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 32);

	//end testing
}

/**
 * @brief the interrupt handler for timer 1
 * 				updates the duty ratios for TIM1 with the RGB data
 * 
 */
void TIM1_UP_TIM16_IRQHandler(void)
{
	//Check whether an overflow event has taken place
	if (TIM1->SR & TIM_SR_UIF)
	{
		//Increment counter
		TIM1->CCR1 = dutyData[counter++];

		//Clear flag UIF to prevent reentrance
		TIM1->SR &= ~TIM_SR_UIF;
	}
}

void SysTick_Init(uint32_t ticks)
{
	SysTick->CTRL = 0;            	// Disable SysTick
	SysTick->LOAD = ticks - 1;    	// Set reload register
	
	// Set interrupt priority of SysTick to least urgency (i.e., largest priority value)
	NVIC_SetPriority(SysTick_IRQn, (1<<__NVIC_PRIO_BITS) -1);
	
	SysTick->VAL = 0;             // Reset the SysTickcounter value
	
	// Select processor clock: 1 = processor clock; 0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	
	// Enables SysTickinterrupt, 1 = Enable, 0 = Disable
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick
	//SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

}

void SysTick_Handler(void)
{
	if (blink1)
	{
		updateLED(rgbBlink1, true);
	}
	else
	{
		updateLED(rgbBlink2, true);
	}
	blink1 = !blink1;
}

/**
 * @brief fills the duty ratio array from the raw RGB data array
 * 
 * @param rgbData the raw RGB data to use for the LEDs
 */
void fillDuty(uint32_t rgbData[LED_COUNT])
{
	// disable the timers
	TIM1->CR1 &= ~TIM_CR1_CEN;
	TIM2->CR1 &= ~TIM_CR1_CEN;

	// set the appropriate duty ratios for each bit
	for (int i = 0; i < LED_COUNT; ++i)
	{
		uint32_t pixel = rgbData[i];
		for (int j = 0; j < BITS_PER_PIXEL; ++j)
		{
			uint32_t bit = ((pixel >> ((BITS_PER_PIXEL - 1) - j)) & 0x1);
			if (bit == 1)
				dutyData[i * BITS_PER_PIXEL + j] = DUTY_1;
			else
				dutyData[i * BITS_PER_PIXEL + j] = DUTY_0;
		}
	}

	// set reset bits for any extras
	for (int i = LED_COUNT * BITS_PER_PIXEL; i < DUTY_LEN; ++i)
	{
		dutyData[i] = DUTY_RESET;
	}
}
