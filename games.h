/**
 * @file games.h
 * @author Adam Fletcher (adam.fletcher@aggiemail.usu.edu), Joshua Brock (joshdb1@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2020-12-15
 * 
 * 
 */

#ifndef GAMES_H
#define GAMES_H

#include <stdio.h>
#include "LCD.h"
#include "uart.h"
#include "TicTacToe.h"

enum Games
{
	QUIT = 0,
	TIC_TAC_TOE,
	CHECKERS,
	SHIP_BATTLE,
	NUM_GAMES
};

uint8_t chooseGame(char c);

void displayScore(int wins, int losses);

void displayWinner(int player);

#endif
