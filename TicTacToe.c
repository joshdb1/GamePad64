/**
 * @file TicTacToe.c
 * @author Adam Fletcher (adam.fletcher@aggiemail.usu.edu), Joshua Brock (joshdb1@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-15
 * 
 * 
 */

#include "TicTacToe.h"
#include "uart.h"
#include "LED.h"

bool tttWin(uint8_t game[3][3]);
void tttChooseSquare(uint8_t game[3][3], 
										 uint8_t player, 
										 char c, 
										 uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT]);
void tttChoices(uint8_t m, 
								uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT], 
								uint32_t b[LED_BOARD_WIDTH][LED_BOARD_HEIGHT],
								uint32_t color);
void tttMoveSquare(uint8_t game[3][3], 
									 uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT], 
									 uint8_t m, 
									 uint8_t player);

uint8_t playTicTacToe(char c)
{
	char won = 0;
	uint8_t m = 0, turn = 0;
	uint8_t game[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

	uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT] = 
										  {{0, 0, W, 0, 0, W, 0, 0},
											{0, 0, W, 0, 0, W, 0, 0},
											{W, W, W, W, W, W, W, W},
											{0, 0, W, 0, 0, W, 0, 0},
											{0, 0, W, 0, 0, W, 0, 0},
											{W, W, W, W, W, W, W, W},
											{0, 0, W, 0, 0, W, 0, 0},
											{0, 0, W, 0, 0, W, 0, 0}};

	//display a to board;
	updateLED((uint32_t *)a, false);

	while (!won)
	{
		//player 1s turn
		if (c != 2)
		{
			tttChooseSquare(game, 1, c, a);
		}
		else /*c==2*/
		{
			m = uartReceive(MIN);
			while (!m)
			{
				m = uartReceive(MIN);
				LCD_DisplayString((uint8_t *)"AGAIN?");
				while ((GPIOA->IDR & 0x2F) == 0)
					;
				//if not center, game over
				if (!(GPIOA->IDR & 0x1))
				{
					LCD_DisplayString((uint8_t *)"OVER!!");
					won = 99;
				}
			}
			if (won == 99)
			{
				continue;
			}
			tttMoveSquare(game, a, m, 1);
		}
		if (tttWin(game))
		{
			won = 1;
			continue;
		}
		turn++;
		if (turn == 9)
		{
			won = 9;
			LCD_DisplayString((uint8_t *)"CAT!!!");
			continue;
		}
		//Player 2s turn
		if (c != 1)
		{
			tttChooseSquare(game, 2, c, a);
		}
		else /*c==1*/
		{
			m = uartReceive(MIN);
			while (!m)
			{
				m = uartReceive(MIN);
				LCD_DisplayString((uint8_t *)"AGAIN?");
				while ((GPIOA->IDR & 0x2F) == 0)
					;
				//if not center, game over
				if (!(GPIOA->IDR & 0x1))
				{
					LCD_DisplayString((uint8_t *)"OVER!!");
					won = 99;
				}
			}
			if (won == 99)
			{
				continue;
			}
			tttMoveSquare(game, a, m, 2);
		}
		if (tttWin(game))
		{
			won = 2;
		}
		turn++;
	}
	return won;
}

//checks to see if either player has won tic tac toe
//returns 1 if the game has been won, or 0 if not
bool tttWin(uint8_t game[3][3])
{
	bool won = 0;
	if (game[0][0] && game[0][0] == game[0][1] && game[0][1] == game[0][2])
	{
		won = 1;
	} //row 0
	else if (game[1][0] && game[1][0] == game[1][1] && game[1][1] == game[1][2])
	{
		won = 1;
	} //row 1
	else if (game[2][0] && game[2][0] == game[2][1] && game[2][1] == game[2][2])
	{
		won = 1;
	} //row 2
	else if (game[0][0] && game[0][0] == game[1][0] && game[1][0] == game[2][0])
	{
		won = 1;
	} //col 0
	else if (game[0][1] && game[0][1] == game[1][1] && game[1][1] == game[2][1])
	{
		won = 1;
	} //col 1
	else if (game[0][2] && game[0][2] == game[1][2] && game[1][2] == game[2][2])
	{
		won = 1;
	} //col 2
	else if (game[0][0] && game[0][0] == game[1][1] && game[1][1] == game[2][2])
	{
		won = 1;
	} //L->R
	else if (game[0][2] && game[0][2] == game[1][1] && game[1][1] == game[2][0])
	{
		won = 1;
	} //R->L
	return won;
}

//Allows a player to choose a spot on tic-tac-toe board
void tttChooseSquare(uint8_t game[3][3], 
										 uint8_t player, 
										 char c, 
										 uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT])
{
	uint8_t m = 5;
	bool chosen = 0;
	uint32_t b[LED_BOARD_WIDTH][LED_BOARD_HEIGHT];
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; ++j)
		{
			b[i][j] = a[i][j];
		}
	}
	while (chosen == 0)
	{
		uint8_t temp = m;
		tttChoices(temp, a, b, B);
		LCD_DisplayString((uint8_t *)"CHOOSE");
		while ((GPIOA->IDR & 0x2F) == 0)
			;
		uint32_t temp2 = GPIOA->IDR & 0x2F;

		switch (temp2)
		{
		//center
		case 0x01:
			//only allow selection if square is available
			if ((game[((m - 1) / 3)][((m - 1) % 3)]) == 0)
			{
				chosen = 1;
			}
			break;
		//left
		case 0x08: //0x02:
			if ((m % 3) != 1)
				m--;
			break;
		//right
		case 0x20: //0x04:
			if ((m % 3) != 0)
				m++;
			break;
		//up
		case 0x04: //0x08:
			if (m >> 2)
				m -= 3;
			break;
		//down
		case 0x02: //0x20:
			if (!(m / 7))
				m += 3;
			break;
		default:
			LCD_DisplayString((uint8_t *)"HIT1!!");
			//			for (volatile int i = 0; i < (SEC << 2); i++)
			//				;
			break;
		}
		tttChoices(temp, a, b, COLOR_RESET);
		while ((GPIOA->IDR & 0x2F) == temp2)
			;
	}
	if (c)
	{
		uartSend(m);
	}
	game[(m - 1) / 3][(m - 1) % 3] = player;
	tttChoices(m, a, a, (player == 1 ? R : G));
	updateLED((uint32_t *)a, false);
}

//sets array b with the squares to flash
void tttChoices(uint8_t m, 
								uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT], 
								uint32_t b[LED_BOARD_WIDTH][LED_BOARD_HEIGHT], 
								uint32_t color)
{
	if (color == COLOR_RESET)
	{
		switch (m)
		{
		case 1:
			b[0][0] = b[0][1] = b[1][0] = b[1][1] = a[0][0];
			break;
		case 2:
			b[0][3] = b[0][4] = b[1][3] = b[1][4] = a[0][3];
			break;
		case 3:
			b[0][6] = b[0][7] = b[1][6] = b[1][7] = a[0][6];
			break;
		case 4:
			b[3][0] = b[3][1] = b[4][0] = b[4][1] = a[3][0];
			break;
		case 5:
			b[3][3] = b[3][4] = b[4][3] = b[4][4] = a[3][3];
			break;
		case 6:
			b[3][6] = b[3][7] = b[4][6] = b[4][7] = a[3][6];
			break;
		case 7:
			b[6][0] = b[6][1] = b[7][0] = b[7][1] = a[6][0];
			break;
		case 8:
			b[6][3] = b[6][4] = b[7][3] = b[7][4] = a[6][3];
			break;
		case 9:
			b[6][6] = b[6][7] = b[7][6] = b[7][7] = a[6][6];
			break;
		}
	}
	else
	{
		switch (m)
		{
		case 1:
			b[0][0] = b[0][1] = b[1][0] = b[1][1] = color;
			break;
		case 2:
			b[0][3] = b[0][4] = b[1][3] = b[1][4] = color;
			break;
		case 3:
			b[0][6] = b[0][7] = b[1][6] = b[1][7] = color;
			break;
		case 4:
			b[3][0] = b[3][1] = b[4][0] = b[4][1] = color;
			break;
		case 5:
			b[3][3] = b[3][4] = b[4][3] = b[4][4] = color;
			break;
		case 6:
			b[3][6] = b[3][7] = b[4][6] = b[4][7] = color;
			break;
		case 7:
			b[6][0] = b[6][1] = b[7][0] = b[7][1] = color;
			break;
		case 8:
			b[6][3] = b[6][4] = b[7][3] = b[7][4] = color;
			break;
		case 9:
			b[6][6] = b[6][7] = b[7][6] = b[7][7] = color;
			break;
		}
	}
	//Display both a[8][8] and b[8][8] here

	updateLEDBlink((uint32_t *)a, (uint32_t *)b);
}

//Accepts message and alters board accordingly
void tttMoveSquare(uint8_t game[3][3], 
									 uint32_t a[LED_BOARD_WIDTH][LED_BOARD_HEIGHT], 
									 uint8_t m,
									 uint8_t player)
{
	game[(m - 1) / 3][(m - 1) % 3] = player;
	tttChoices(m, a, a, player == 1 ? R : G);
}