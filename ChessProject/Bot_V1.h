#pragma once
#include "board.h"

class Bot_V1 {
	Board* board;
	bool bot;
	bool player = !bot;

public:

	Bot_V1(Board* pb,bool botColor) : board(pb), bot(botColor) {}

	int piecePunctuation(int );
	int EvaluateGame(bool );
	
	vector<Move> generateAllMoves( bool);


	void orderMoves(vector<Move>&);

	int minimax(int, int*, int*, int*, bool, vector <Move>&);
	Move botMove(bool);

};
