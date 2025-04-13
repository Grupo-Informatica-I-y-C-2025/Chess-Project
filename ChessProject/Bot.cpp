#pragma once
#include "Bot.h"
#include <random>
#include <chrono>


#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define EMPTY_CELL 6
#define WHITE 0
#define BLACK 1
#define NONE 2


int Bot::piecePunctuation(int type) {
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

int Bot::EvaluateGame(bool turn) {
	int score = 0;
	// Evaluación de material
	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & 1ULL << sq) {
			int type = board->BitboardGetType(sq);
			int color = board->BitboardGetColor(sq);
			score += ( color == bot) ? piecePunctuation(type) : -piecePunctuation(type);
		}
	}

	// Bonus por jaque al oponente
	bool inCheck = board->scanChecks(!turn);
	if (inCheck) score += (turn == bot) ? 5000 : -5000;

	return score;
}

vector<Move> Bot::generateAllMoves( bool color) {

	vector<Move> legalMoves = board->generateAllMoves(color);
	orderMoves(legalMoves);
	return legalMoves;
}

void Bot::orderMoves(vector<Move>& moves) {
	std::sort(moves.begin(), moves.end(), [this](const Move& a, const Move& b) {
		/*// Prioridad 1: Movimientos que dan jaque
		bool aCheck = scanChecksStatic(a);
		bool bCheck = scanChecksStatic(b);
		if (aCheck != bCheck) return aCheck > bCheck;*/

		
		// Prioridad 2: Ganancia de material
		int aGain = piecePunctuation(board->BitboardGetType(a.targetSquare)) - piecePunctuation(board->BitboardGetType(a.sourceSquare));
		int bGain = piecePunctuation(board->BitboardGetType(b.targetSquare)) - piecePunctuation(board->BitboardGetType(b.sourceSquare));
		return aGain > bGain;
		});
}

int Bot::minimax( int depth, int* n, int* alpha, int* beta, bool maximizingPlayer, vector <Move>& moves) {

	int bestEval =(maximizingPlayer)? -100000: 100000;
	int alpha1 = *alpha, beta1 = *beta;
	bool color = (maximizingPlayer)? bot : player;


	if (depth == 0 || board->scanCheckMate(color)) {
		int eval = EvaluateGame(color);
		if (board->scanCheckMate(!color)) eval += (maximizingPlayer) ? 100000 : -100000;
		return  eval;
	}

	for ( auto& move : generateAllMoves(color)) {
		if (beta1 <= alpha1)  break;

		MoveResult result = board->makeMove(move);
		if (!result.success)continue;

		//check if result in checkmate
		if (!board->scanChecks( color)) {
			int eval = minimax( depth - 1, n, &alpha1, &beta1, !maximizingPlayer, moves);

			// [5] Actualizar alpha/beta con logs
			if (maximizingPlayer) {
				bestEval = max(bestEval, eval);
				alpha1 = max(alpha1, eval);
			}
			else {
				bestEval = min(bestEval, eval);
				beta1 = min(beta1, eval);
			}
			if (*n == depth && eval<10000 && eval > -10000) {
				auto moveD1 = move;
				moveD1.score = eval;
				moves.push_back(moveD1);
			}
		}
		//undo
		board->undoMove();
		if (beta1 <= alpha1)  break;

	}
	return bestEval;

}

Move Bot::botMove(bool turn) {

	vector <Move> bestmoves;
	auto start = std::chrono::high_resolution_clock::now();
	//int bestscore = (turn == WHITE) ? 10000 : -10000;


	int alpha = -10000, beta = 10000;
	int depth = 5;

	int score = minimax(depth, &depth, &alpha, &beta, (turn == bot) ? true : false, bestmoves);


	/*for (int depth = 1; depth <= 5; depth++) {
		//cout << "\n" << depth;
		int alpha = -10000, beta = 10000;
		vector<movement> moves;
		int score = minimax(depth, &depth, &alpha, &beta, (turn == bot) ? true : false, moves);
		if (score > bestscore) {
			bestscore = score;
			bestmoves.clear();
			for (const auto& m : moves) {
				if (bestscore == m.score) {
					bestmoves.push_back(m);
				}
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() > 2000) {
			cout << "\nTiempo: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
			break;
		}
	}*/
	//cout<<"\n best score:"<< bestscore;
	


	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<size_t> dis(0, bestmoves.size() - 1);

	Move bestmove = { bestmoves[dis(gen)] };

	return  bestmove;
}
