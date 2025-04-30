#pragma once

#include "board.h"
#include "Bot_V1.h"
#include "Bot_V2.h"
#include "Bot_V3.h"
#include "Bot_V4.h"
#include "Bot_SB.h"
#include "Leela.h"

class BoardGL;

class Game {
	friend class BoardGL;
protected:
	
	Board m_board;
	Board* board;

	Bot_V4 bot4;
	Bot_V3 bot3;
	Bot_V2 bot2;
	Bot_V1 bot1;

	LC0 leela;
	int N;
	int M;

	Move movement{-1,-1};

public:
	bool bot = 1;
	bool player = 0;
	bool generated = 0;

	Game(int n) :m_board(n), board(& m_board), bot3(board), bot2(board),bot4(board), bot1(board,bot){
		
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
	bool playTurn();
	
	void saveGame();
	void loadSavedGame();

	std::vector<int> listSavedGames() const;
	int getNextSaveNumber() const;


private:
	bool game_over = false;
	std::vector<std::string> fenPositions;

	int totalGames;
	int whiteWins;
	int blackWins;
	int draws;

public:

	void loadFENPositions(const std::string& );
	void runSimulation(int );
	void printStats() const;
};
