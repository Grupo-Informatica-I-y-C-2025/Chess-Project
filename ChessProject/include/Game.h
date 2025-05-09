#pragma once
#include "board.h"
#include "Bot_V4.h"

class BoardGL;

class Game {
	friend class BoardGL;
protected:
	
	Board* board;

	Bot_V4 bot4;

	int N;
	int M;

	Move movement{-1,-1};

public:
	bool bot = 1;
	bool player = 0;
	bool generated = 0;

	Game(int n) {
		
		board = new Board(n);

		N = board->getSizeY();
		M = board->getSizeX();
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
