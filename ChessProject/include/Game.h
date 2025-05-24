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
	bool bot;
	bool pvp;
	bool generated = 0;

	Game(int n,int b) {
		
		board = new Board(n);

		N = board->getSizeY();
		M = board->getSizeX();
		game_over = false;

		if(b==2){
			pvp = true;
			bot = false;
		}else{
			pvp = false;
			bot = (bool)b;
		}

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
	void loadSavedGame(int);

	static std::vector<int> listSavedGames() ;
	static int getNextSaveNumber() ;
	static int LoadType(int );



private:
	bool game_over;
	std::vector<std::string> fenPositions;

	int totalGames;
	int whiteWins;
	int blackWins;
	int draws;

public:
	void printStats() const;

	bool getTurn(){return board->currentState.turn;}
};
