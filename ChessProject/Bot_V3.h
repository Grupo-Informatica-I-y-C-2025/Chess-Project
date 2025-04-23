#pragma once
#include "board.h"



class Bot_V3 {
	Board* board;

public:
	struct search_result {
		int evaluation;
		vector<Move> moves;

	};


	Bot_V3(Board* pb) : board(pb) {}

	int piecePunctuation(int);
	int EvaluateGame(bool);

	int materialEvaluation(bool);
	int gamePhase();
	int positionalEvaluation(bool);
	int centerControlEvaluation(bool);
	int mobilityEvaluation(bool);
	int kingSafetyEvaluation(bool);
	int pawnStructureEvaluation(bool);
	int pieceCoordinationEvaluation(bool);

	vector<Move> generateAllMoves(bool);
	vector<Move> generateCaptureMoves(bool);


	void orderMoves(vector<Move>&);
	void sortMovesByEvaluation(vector<Move>&, bool);

	search_result SearchMoves(int, int, int, bool);
	search_result SearchCaptures(int , int , int , bool );

	Move botMove(bool);

};
