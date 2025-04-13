#pragma once
#include "board.h"
#include "Bot.h"

class Game {
protected:
	Move movement;
	Board* board;
	Bot machine;
	int N;
	int M;


public:
	bool bot = 1;
	bool player = 0;

	Game(Board* pb) :board(pb), machine(pb, bot) {
		N = pb->getSizeY();
		M = pb->getSizeX();		//Grid NxN
		;

	}

	//reglas de movimiento
	void activate(int xcell_sel, int ycell_sel);
	void registerCall(int xcell_sel, int ycell_sel);
	void unselectAll();


	//public game functions
	bool isgame_over() { return game_over; }
	void makeMove();
	

private:


	bool game_over = false;
	bool action = false;
	int src[2] = { -1,-1 }, dest[2] = { -1,-1 };



};
