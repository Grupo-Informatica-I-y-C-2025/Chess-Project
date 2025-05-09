#include "Bot_V4.h"
#include "generator.h"
#include "evaluation.h"
#include <random>
#include <chrono>

const int QUIESCE_DEPTH = 3;


static int totalNodes;
Bot_V4::search_result Bot_V4::SearchMoves(int depth, int alpha, int beta, Board& board) {
	totalNodes++;

	if (depth == 0)
		return SearchQuiescent(alpha,beta,board);

	bool maximizing = (board.currentState.turn == WHITE);

	int bestEval = maximizing ? INIT_MIN : INIT_MAX;
	Move bestMove = { 0,0,NONE,DEFAULT };

	auto moves = Generator::generateAllMoves(board.currentState.turn,board);
	Generator::orderMoves(moves,board);


 	bool pvNode = (beta - alpha) > 1;

	for (size_t i = 0; i < moves.size(); ++i) {

        auto& m = moves[i];

		auto res = Generator::makeMove(m,board);
		if (!res.success) continue;

		 search_result sr;
        if (pvNode && i == 0) {
            sr = SearchMoves(depth-1, alpha, beta, board);
        } else {
            sr = SearchMoves(depth-1, alpha-20, alpha, board);
            if (sr.evaluation > alpha)
                sr = SearchMoves(depth-1, alpha, beta, board);
        }

		Generator::undoMove(board);

		if ((maximizing && sr.evaluation > bestEval) || (!maximizing && sr.evaluation < bestEval)) {
			bestEval = sr.evaluation;
			bestMove = m;
		}
		if (maximizing) alpha = max(alpha, bestEval);
		else beta = min(beta, bestEval);
		if (beta <= alpha) break;
	}

	return { bestEval, bestMove };
}

Bot_V4::search_result Bot_V4::SearchQuiescent(int alpha, int beta,Board& board) {
	totalNodes++;

	bool maximizing = (board.currentState.turn == 0);
	int standPat = Evaluation::EvaluateGame(board);

	if (standPat >= beta) return { standPat, Move{0,0,NONE,DEFAULT} };
	if (standPat > alpha) alpha = standPat;

	auto moves = Generator::generateQuiescientMoves(board.currentState.turn,board);

	//posiciones estables sal de busqueda quiesciente
	if (moves.size() <= 1) 
        return { standPat, Move{0,0,NONE,DEFAULT} };

	int bestEval = standPat;
	Move bestMove = Move{ 0,0,NONE,DEFAULT };

	for (auto& m : moves) {
		if (staticExchangeEval(m,board) < -20) continue;

		auto res = Generator::makeMove(m,board);
		if (!res.success) continue;

		if (m.targetSquare != res.targetSquare) {
            Generator::undoMove(board);
            continue;
        }

		auto sr = SearchQuiescent(alpha, beta,board);

		Generator::undoMove(board);

		if (maximizing) alpha = std::max(alpha, bestEval);
		else beta = std::min(beta, bestEval);

		if (beta <= alpha) break;
	}

	return { bestEval ,};
}

int Bot_V4::staticExchangeEval(const Move& move,Board& board) {
	int target = move.targetSquare;
	int gain[32];
	int depth = 0;

	gain[depth] = board.piecePunctuation(board.BitboardGetType(target));

	Bitboard attackersWhite = board.attackersTo(target, WHITE);
	Bitboard attackersBlack = board.attackersTo(target, BLACK);

	bool side = board.currentState.turn;

	Bitboard fromBB = 1ULL<<move.sourceSquare;
	if (side == WHITE)
		attackersWhite &= ~fromBB;
	else
		attackersBlack &= ~fromBB;

	while (true) {
		side = (side == WHITE) ? BLACK : WHITE;
		Bitboard attackers = (side == WHITE) ? attackersWhite : attackersBlack;

		if (attackers == 0)
			break;

		int from = selectLeastValuableAttacker(attackers, side,board);
		int pt = board.BitboardGetType(from);

		depth++;
		gain[depth] = board.piecePunctuation(pt) - gain[depth - 1];

		if (gain[depth] < 0)
			break;

		Bitboard fromBB = 1ULL<<from;
		if (side == WHITE)
			attackersWhite &= ~fromBB;
		else
			attackersBlack &= ~fromBB;
	}

	while (depth > 0) {
		gain[depth - 1] = -std::max(-gain[depth - 1], gain[depth]);
		depth--;
	}

	return gain[0];
}

int Bot_V4::selectLeastValuableAttacker(Bitboard attackers, bool color,Board& board) {
	int minValue = 999999; // Valor inicial alto
	int selectedSquare = -1;

	while (attackers) {
		int sq = portable_ctzll(attackers);
		attackers &= attackers - 1; // Eliminar el bit procesado

		// Obtener tipo de pieza y su valor
		int type = board.BitboardGetType(sq);
		int value = board.piecePunctuation(type);

		// Actualizar mínimo
		if (value < minValue || (value == minValue && sq < selectedSquare)) {
			minValue = value;
			selectedSquare = sq;
		}
	}

	return selectedSquare;
}

Move Bot_V4::botMove(bool turn,Board& board) {
	totalNodes = 0;

	Move bestMove;
	search_result sr;
	int maxDepth = 5; // ajustar según rendimiento
	for (int d = 1; d <= maxDepth; ++d) {
		cout << "\n--- Search Depth " << d << " ---" << endl;
		sr = SearchMoves(d, INIT_MIN, INIT_MAX,board);
		cout << "BOT4 Evaluación: " << sr.evaluation << endl;
		cout << "BOT4 Nodos: " << totalNodes << endl;
		bestMove = sr.bestMove;
	}
	return bestMove;
}
