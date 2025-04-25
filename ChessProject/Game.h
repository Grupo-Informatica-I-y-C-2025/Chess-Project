#pragma once
#include "board.h"
#include "Bot_V1.h"
#include "Bot_V2.h"
#include "Bot_V3.h"
#include "Bot_V4.h"
#include "Bot_SB.h"

class BoardGL;

class Game {
	friend class BoardGL;
protected:
	
	Board m_board;
	Board* board;
	Bot_V2 black_bot;
	Bot_V3 white_bot;
	int N;
	int M;

	Move movement{-1,-1};

public:
	bool bot = 1;
	bool player = 0;
	bool generated = 0;

	Game(int n) :m_board(n), board(& m_board), black_bot(board),white_bot(board) {
		
		N = m_board.getSizeY();
		M = m_board.getSizeX();
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
};
