#pragma once
#include "board.h"
#include "Bot_V1.h"
#include "Bot_V2.h"
#include "Bot_V3.h"
class Game {
protected:
	Move movement;
	Board* board;
	Bot_V1 machine1;
	Bot_V3 machine2;
	int N;
	int M;


public:
	bool bot = 1;
	bool player = 0;
	bool generated = 0;

	Game(Board* pb) :board(pb), machine1(pb,bot), machine2(pb) {
		N = pb->getSizeY();
		M = pb->getSizeX();		//Grid NxN
		movement.sourceSquare = -1;
		movement.targetSquare = -1;

	}

	//reglas de movimiento
	void activate(int xcell_sel, int ycell_sel);
	void registerCall(int xcell_sel, int ycell_sel);
	void unselectAll();


	//public game functions
	void scanEndGame(bool);
	bool isgame_over() { return game_over; }
	void playTurn();
	

private:


	bool game_over = false;
	bool action = false;
};
