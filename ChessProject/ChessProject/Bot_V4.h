#pragma once
#include "board.h"

class Bot_V4 {
	Board* board;

private:
	std::unordered_map<uint64_t, int> evalCache;
	std::vector<uint64_t> history;

	// Tablas Zobrist
	uint64_t zobristTable[64][12];
	uint64_t zobristTurn;
	uint64_t zobristCastling[4];
	uint64_t zobristEnPassant[8];

	// Caché de transposición
	struct TTEntry {
		int depth;
		int evaluation;
		enum BoundType { EXACT, LOWERBOUND, UPPERBOUND } flag;
	};
	std::unordered_map<uint64_t, TTEntry> transpositionTable;

	// Hash actual
	uint64_t currentHash;
	void initializeZobrist();
	void updateHash(const Move& move, const MoveResult& result);
	void revertHash(const Move& move, const MoveResult& result);
	uint64_t computeFullHash();
	void clearTranspositionTable() {
		transpositionTable.clear();
		history.clear();
	}

public:
	struct search_result {
		int evaluation;
		Move bestMove;
	};

	Bot_V4(Board* pb) : board(pb) {
		static bool inited = false;
		transpositionTable.reserve(1 << 20);
		if (!inited) {
			initializeZobrist();
			inited = true;
		}
		currentHash = computeFullHash();
		history.clear();
		history.push_back(currentHash);
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
