#pragma once
#include "board.h"

class Bot_V4 {

public:

	struct search_result {
		int evaluation;
		Move bestMove;

		search_result operator-(){
			evaluation = evaluation* -1;
			return *this;
		}
	};

	Bot_V4(){}

	Move botMove(bool,Board&);

private:



	search_result SearchMoves(int, int, int,Board&);
	search_result SearchQuiescent( int, int,Board&);
	search_result SearchChecks(int, int,Board&);

	int staticExchangeEval(const Move&,Board& );
	int selectLeastValuableAttacker(Bitboard, bool,Board&);
	
};
