#pragma once
#include "board.h"

class Bot_V4 {
	Board* board;

public:
	struct search_result {
		int evaluation;
		Move bestMove;
	};

	Bot_V4(Board* pb) : board(pb) {
	}

	Move botMove(bool);

private:

	int piecePunctuation(int);
	int EvaluateGame();
	int evaluateColor(bool );

	int materialEvaluation(bool);
	int pieceActivityEvaluation(bool );

	int centerControlEvaluation(bool);
	int mobilityEvaluation(bool);
	int kingSafetyEvaluation(bool);
	int pawnStructureEvaluation(bool);
	int bishopPairEvaluation(bool);
	int rookOpenFileEvaluation(bool);
	int badBishopEvaluation(bool);
	int seventhRankEvaluation(bool );

	int gamePhase();
	int positionalEvaluation(bool);

	vector<Move> generateAllMoves(bool);
	vector<Move> generateQuiescentMoves(bool);
	vector<Move> generateCheckMoves(bool);


	void orderMoves(vector<Move>&);

	search_result SearchMoves(int, int, int);
	search_result SearchQuiescent( int, int);
	search_result SearchChecks(int, int);
	int staticExchangeEval(const Move& );
	int selectLeastValuableAttacker(Bitboard, bool);


};
