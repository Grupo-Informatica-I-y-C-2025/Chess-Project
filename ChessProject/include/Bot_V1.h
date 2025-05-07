#pragma once
#include "board.h"
#include <chrono>

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

const int INIT_MAX = -100000000; // -10^9
const int INIT_MIN = 100000000;  // 10^9


inline int Bot_V1::piecePunctuation(int type) {
	switch (type)
	{
	case PAWN:

		return 100;
		break;

	case ROOK:

		return 500;
		break;

	case BISHOP:

		return 300;
		break;

	case KNIGHT:

		return 300;
		break;

	case QUEEN:

		return 900;
		break;

	case KING:

		return 0;
		break;


	case EMPTY_CELL:

		return 0;
		break;

	default:

		return 0;
		break;
	}
}

inline int Bot_V1::EvaluateGame(bool turn) {
	int score = 0;
	// Evaluación de material
	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & 1ULL << sq) {
			int type = board->BitboardGetType(sq);
			int color = board->BitboardGetColor(sq);
			score += (color == bot) ? piecePunctuation(type) : -piecePunctuation(type);
		}
	}

	// Bonus por jaque al oponente
	bool inCheck = board->scanChecks(!turn);
	if (inCheck) score += (turn == bot) ? 5000 : -5000;

	return score;
}

inline vector<Move> Bot_V1::generateAllMoves(bool color) {
	vector<Move> legalMoves;

	legalMoves = board->generateAllMoves(color);

	orderMoves(legalMoves);
	return legalMoves;
}

inline void Bot_V1::orderMoves(vector<Move>& moves) {
	for (size_t i = 0; i < moves.size(); ++i) {
		int maxIdx = i;
		for (size_t j = i + 1; j < moves.size(); ++j) {
			int gainCurrent = piecePunctuation(board->BitboardGetType(moves[j].targetSquare));
			int gainMax = piecePunctuation(board->BitboardGetType(moves[maxIdx].targetSquare));
			if (gainCurrent > gainMax) maxIdx = j;
		}
		swap(moves[i], moves[maxIdx]);
	}
}

inline int Bot_V1::minimax(int depth, int* n, int* alpha, int* beta, bool maximizingPlayer, vector<Move>& moves) {
	int bestEval = (maximizingPlayer) ? INIT_MAX : INIT_MIN;
	int alpha1 = *alpha, beta1 = *beta;
	bool color = (maximizingPlayer) ? bot : player;

	if (depth == 0) {
		int eval = EvaluateGame(color);
		if (board->scanCheckMate(!color)) {
			eval = (maximizingPlayer) ? 1000000 : -1000000;
		}
		return eval;
	}
	if (board->scanCheckMate(color)) {
		int eval = (maximizingPlayer) ? -1000000 : +1000000;
		return eval;
	}

	vector<Move> legalmoves = generateAllMoves(color);
	if (legalmoves.empty()) return (maximizingPlayer) ? *alpha : *beta;

	for (int i = 0; i < legalmoves.size(); ++i) {
		Move& move = legalmoves[i];


		MoveResult result = board->makeMove(move);
		if (!result.success) continue;


		int eval = minimax(depth - 1, n, &alpha1, &beta1, !maximizingPlayer, moves);
		if (depth == *n)legalmoves[i].score = eval;
		if (maximizingPlayer) {
			bestEval = max(bestEval, eval);
			alpha1 = max(alpha1, eval);

		}
		else {
			bestEval = min(bestEval, eval);
			beta1 = min(beta1, eval);
		}

		board->undoMove();

		if (beta1 <= alpha1) {
			break;
		}
	}
	moves = legalmoves;
	return bestEval;
}

inline Move Bot_V1::botMove(bool turn) {

	vector <Move> bestmoves;
	auto start = chrono::high_resolution_clock::now();
	//int bestscore = (turn == WHITE) ? 10000 : -10000;


	int alpha = -10000, beta = 10000;
	int depth = 4;

	int score = minimax(depth, &depth, &alpha, &beta, (turn == bot) ? true : false, bestmoves);

	//cout<<"\n best score:"<< bestscore;

	if (bestmoves.empty()) {
		// Si no hay movimientos, devolver un movimiento nulo o lanzar error
		cout << "\n no hay ningun movimiento legal detectado";
		Move defaultMove = { 0, 0, NONE, DEFAULT };
		return defaultMove;
	}
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<size_t> dis(0, bestmoves.size() - 1);


	Move bestmove = { bestmoves[dis(gen)] };

	return  bestmove;
}
