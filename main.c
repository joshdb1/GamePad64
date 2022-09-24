/**
 * @file main.c
 * @author Joshua Brock (joshdb1@gmail.com), Adam Fletcher (adam.fletcher@aggiemail.usu.edu)
 * @brief The entry point for our program
 * @version 0.1
 * @date 2020-12-15
 * 
 * 
 */

#include "stm32l476xx.h"
#include "LCD.h"
#include "keypad.h"
#include "LED.h"
#include "games.h"
#include "uart.h"

/****************************************************************************************************************
Pseudocode
Begin
	initialize control mechanisms
	initialize LED_array
	initialize LCD
	check if connected to other uC
	forever loop
		decide game to play
		play game chosen
		update total games won
	endloop
End
****************************************************************************************************************/

int main(void)
{ //Begin
	uint8_t won = 0, connected = 0;
	int wins = 0, losses = 0;

	//initialize control mechanisms
	joystickInit();

	//initialize LED_array
	LED_Init();

	uint32_t startup1[LED_BOARD_WIDTH][LED_BOARD_HEIGHT] =
			{{B, Bl, Bl, Bl, R, R, R, R},
			 {B, Bl, Bl, Bl, R, Bl, Bl, R},
			 {B, Bl, Bl, Bl, R, Bl, Bl, R},
			 {B, B, B, B, R, R, R, R},
			 {B, Bl, Bl, B, Bl, Bl, Bl, R},
			 {B, Bl, Bl, B, Bl, Bl, Bl, R},
			 {B, Bl, Bl, B, Bl, Bl, Bl, R},
			 {B, Bl, Bl, B, Bl, R, R, R}};

	updateLED((uint32_t *)startup1, false);

	//initialize LCD
	LCD_Initialization();

	//Prompt for if connection desired
	LCD_DisplayString((uint8_t *)"CNECT?");
	while ((GPIOA->IDR & 0x2F) == 0)
		;
	if (GPIOA->IDR & 0x1)
	{
		uartInit();
		//check if connected to other uC
		connected = uartConnect();
	}
	LCD_Clear();

	//forever loop
	while (1)
	{
		//	decide game to play
		switch (chooseGame(connected))
		{
			//	play game chosen
		case TIC_TAC_TOE:
			won = playTicTacToe(connected);
			break;
			//			case 2:
			//				won = playCheckers(connected);
			//				break;
			//			case 3:
			//				won = playBattleship(connected);
			//				break;
		default:
			continue;
		}

		// display the winner
		displayWinner(won);

		// delay 4 seconds
		delay(40000000);

		//clearLED();

		//	update total games won
		if (won == 1)
		{
			wins += 1;
		}
		else if (won == 2)
		{
			losses += 1;
		}
		displayScore(wins, losses);

		delay(40000000);
		//endloop
	}

	clearLED();

} //End
