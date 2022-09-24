/**
 * @file games.c
 * @author Adam Fletcher (adam.fletcher@aggiemail.usu.edu)
 * @brief 
 * @version 0.1
 * @date 2020-12-15
 * 
 * 
 */

#include "games.h"
#include "LED.h"

static char* gameString[NUM_GAMES] =
	{ "QUIT  ",
		"TICTAC",
		"CHCKRS",
		"SHPBTL"
	};
	
static uint32_t sampleBoards[NUM_GAMES * 2][LED_BOARD_WIDTH][LED_BOARD_HEIGHT] =
{
	// QUIT
	{ { B, Bl, Bl, Bl, R, R, R, R },
	  { B, Bl, Bl, Bl, R, Bl, Bl, R },
	  { B, Bl, Bl, Bl, R, Bl, Bl, R },
	  { B, B, B, B, R, R, R, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, R, R, R } },
	
	{ { B, Bl, Bl, Bl, R, R, R, R },
	  { B, Bl, Bl, Bl, R, Bl, Bl, R },
	  { B, Bl, Bl, Bl, R, Bl, Bl, R },
	  { B, B, B, B, R, R, R, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, Bl, Bl, R },
	  { B, Bl, Bl, B, Bl, R, R, R } },
	
	// TIC TAC TOE
	{ {0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0},
		{W, W, W, W, W, W, W, W},
		{0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0},
		{W, W, W, W, W, W, W, W},
		{0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0} },
	
	{ {0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0},
		{W, W, W, W, W, W, W, W},
		{0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0},
		{W, W, W, W, W, W, W, W},
		{0, 0, W, 0, 0, W, 0, 0},
		{0, 0, W, 0, 0, W, 0, 0} },
	
	// CHECKERS
	{ {0, B, 0, B, 0, B, 0, B},
		{B, 0, B, 0, B, 0, B, 0},
		{0, B, 0, B, 0, B, 0, B},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{R, 0, R, 0, R, 0, R, 0},
		{0, R, 0, R, 0, R, 0, R},
		{R, 0, R, 0, R, 0, R, 0} },
	
	{ {0, B, 0, B, 0, B, 0, B},
		{B, 0, B, 0, B, 0, B, 0},
		{0, 0, 0, B, 0, B, 0, B},
		{B, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0},
		{R, 0, R, 0, R, 0, R, 0},
		{0, R, 0, R, 0, R, 0, R},
		{R, 0, R, 0, R, 0, R, 0} },
	
	// SHIP BATTLE 64
	{ {SB, W, W, SB, SB, SB, R, SB},
		{SB, SB, SB, SB, W, SB, R, SB},
		{SB, SB, SB, SB, W, SB, SB, SB},
		{SB, W, W, W, W, SB, SB, SB},
		{SB, SB, SB, SB, W, SB, SB, SB},
		{SB, SB, SB, SB, SB, SB, SB, SB},
		{SB, SB, W, W, W, W, W, SB},
		{SB, SB, SB, SB, SB, SB, SB, SB} },
	
	{ {SB, W, W, SB, SB, SB, R, SB},
		{SB, SB, SB, SB, W, SB, R, SB},
		{SB, SB, SB, SB, W, SB, SB, SB},
		{SB, R, R, W, W, SB, SB, SB},
		{SB, SB, SB, SB, W, SB, SB, SB},
		{SB, SB, SB, SB, SB, SB, SB, SB},
		{SB, SB, W, W, W, W, W, SB},
		{SB, SB, SB, SB, SB, SB, SB, SB} }
	
};

uint8_t chooseGame(char c)
{
	//char choice = 0;
	
	enum Games choice = QUIT;
	
	bool chosen = false;

	while (!chosen)
	{
		LCD_DisplayString((uint8_t *)gameString[choice]);
		updateLEDBlink((uint32_t*)sampleBoards[choice * 2], (uint32_t*)sampleBoards[choice * 2 + 1]);
		while ((GPIOA->IDR & 0x2F) == 0)
			;
		uint32_t temp2 = GPIOA->IDR & 0x2F;
		
		switch (temp2)
		{
		//center
		case 0x01:
			chosen = true;
			break;
		//left
		case 0x08: //0x02:
			choice--;
			break;
		//right
		case 0x20: //0x04:
			choice++;
			break;
		default:
			break;
		}
		if (choice >= NUM_GAMES) choice = 0;
		while ((GPIOA->IDR & 0x2F) == temp2)
			;
	}

	return choice;
}

void displayScore(int wins, int losses)
{
	uint8_t ptr[7];
	sprintf((char *)ptr, "W%d L%d ", wins, losses);
	LCD_DisplayString(ptr);
}

void displayWinner(int player)
{
	uint8_t ptr[7];
	
	switch (player)
	{
		case 1:
		case 2:
			sprintf((char *)ptr, "P%d WON", player);
			break;
		default:
			sprintf((char *)ptr, "DRAW  ");
			break;
	}
	
	LCD_DisplayString(ptr);
	
}
