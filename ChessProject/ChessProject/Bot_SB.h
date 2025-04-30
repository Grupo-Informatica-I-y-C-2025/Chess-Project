#pragma once
#include "board.h"
#include <chrono>


class Bot_SB {
	Board* board;

public:

	struct search_result {
		int evaluation;
		vector<Move> moves;
	};

	Bot_SB(Board* pb) : board(pb) {}

	int piecePunctuation(int);
	int EvaluateGame(bool);

	int materialEvaluation(bool);
	int defenseEvaluation(bool);

	vector<Move> generateAllMoves(bool);
	vector<Move> generateCaptureMoves(bool);


	void orderMoves(vector<Move>&);
	void sortMovesByEvaluation(vector<Move>&, bool);

	search_result SearchMoves(int, int, int, bool);
	search_result SearchCaptures(int, int, int, bool);

	Move botMove(bool);

};

#define INIT_MAX  -100000000
#define INIT_MIN  100000000

inline int Bot_SB::piecePunctuation(int type) {
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

inline int Bot_SB::EvaluateGame(bool turn) {
	int score = 0;

	// Calcular puntuación absoluta
	score += materialEvaluation(turn) * 2;
	score += defenseEvaluation(turn) * 10;

	return 	(turn == WHITE) ? score : -score;;
}

inline int Bot_SB::materialEvaluation(bool turn) {
	int score = 0;
	// Evaluación de material
	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & 1ULL << sq) {
			int type = board->BitboardGetType(sq);
			int color = board->BitboardGetColor(sq);
			score += (color == turn) ? piecePunctuation(type) : -piecePunctuation(type);
		}
	}
	return score;
}

inline int Bot_SB::defenseEvaluation(bool turn) {
	int score = 0;
	Bitboard pieces = board->getPieces(turn);
	Bitboard defense = board->getAttackMap(turn);
	Bitboard threats = board->getAttackMap(!turn);
	// Evaluación de material
	while (pieces) {
		int sq = board->portable_ctzll(pieces);
		int type = board->BitboardGetType(sq);
		if ((1ULL << sq) & defense)score += piecePunctuation(type);
		if ((1ULL << sq) & threats)score -= piecePunctuation(type);
		pieces &= pieces - 1;
	}
	return score;
}

inline vector<Move> Bot_SB::generateAllMoves(bool color) {
	vector<Move> legalMoves;

	legalMoves = board->generateAllMoves(color);

	orderMoves(legalMoves);
	return legalMoves;
}

inline vector<Move> Bot_SB::generateCaptureMoves(bool color) {
	vector<Move> legalMoves;

	legalMoves = board->generateCaptureMoves(color);

	orderMoves(legalMoves);
	return legalMoves;
}

inline void Bot_SB::orderMoves(vector<Move>& moves) {
	if (moves.empty()) return;

	// Obtener el color del jugador que realiza los movimientos
	bool color = board->BitboardGetColor(moves[0].sourceSquare);

	// Precalcular puntuaciones
	vector<pair<int, int>> scoredMoves; // (score, index)
	for (size_t i = 0; i < moves.size(); ++i) {
		int score = 0;
		bool isCheck = false;
		bool isCapture = false;
		int victimValue = 0;

		// 1. Verificar si el movimiento da jaque
		board->makeMove(moves[i]);
		isCheck = board->scanChecks(!color); // Verificar jaque al oponente
		board->undoMove();

		if (isCheck) {
			score = 50000;
			if (board->scanCheckMate(!color)) score += 1000000;
		}
		else {
			// 2. Verificar si es captura (normal o al paso)
			Bitboard targetBB = 1ULL << moves[i].targetSquare;
			isCapture = (board->getPieces(!color) & targetBB) ||
				(moves[i].pieceType == PAWN && (board->currentState.enPassant & targetBB));

			if (isCapture) {
				// Calcular MVV-LVA
				if (board->getPieces(!color) & targetBB) {
					victimValue = piecePunctuation(board->BitboardGetType(moves[i].targetSquare));
				}
				else { // Captura al paso
					victimValue = 100; // Valor de peón
				}
				int aggressorValue = piecePunctuation(moves[i].pieceType);
				score = 10000 + (victimValue - aggressorValue); // MVV-LVA
			}
		}

		scoredMoves.emplace_back(score, i);
	}

	// Ordenar movimientos por puntuación descendente
	sort(scoredMoves.begin(), scoredMoves.end(), [](const auto& a, const auto& b) {
		return a.first > b.first;
		});

	// Reconstruir el vector ordenado
	vector<Move> orderedMoves;
	for (const auto& sm : scoredMoves) {
		orderedMoves.push_back(moves[sm.second]);
	}
	moves = move(orderedMoves);
}

inline Bot_SB::search_result Bot_SB::SearchMoves(int depth, int alpha, int beta, bool maximizingPlayer) {
	int bestEval = (maximizingPlayer) ? INIT_MAX : INIT_MIN;
	bool color = (maximizingPlayer) ? WHITE : BLACK;
	vector<Move> moves;
	if (depth == 0) {
		search_result qsearch = SearchCaptures(2, alpha, beta, maximizingPlayer);
		return { qsearch.evaluation, {} };
	}

	if (board->scanCheckMate(!color)) {
		return { maximizingPlayer ? 1000000 - depth : -1000000 + depth, {} };
	}

	///BUSQUEDA DE MOVIMIENTOS DE CAPTURA///
	for (Move& move : generateCaptureMoves(color)) {
		MoveResult result = board->makeMove(move);
		if (!result.success) continue;

		search_result search = SearchCaptures(3, alpha, beta, !maximizingPlayer);
		move.score = search.evaluation;
		moves.push_back(move);
		board->undoMove();

		if (maximizingPlayer) {
			if (search.evaluation > bestEval) bestEval = search.evaluation;
			alpha = max(alpha, bestEval);
		}
		else {
			if (search.evaluation < bestEval) bestEval = search.evaluation;
			beta = min(beta, bestEval);
		}

		if (beta <= alpha) break;
	}

	///BUSQUEDA DE MOVIMIENTOS NORMALES///
	for (Move& move : generateAllMoves(color)) {
		MoveResult result = board->makeMove(move);
		if (!result.success) continue;

		search_result search = SearchMoves(depth - 1, alpha, beta, !maximizingPlayer);
		move.score = search.evaluation;
		moves.push_back(move);
		board->undoMove();

		if (maximizingPlayer) {
			if (search.evaluation > bestEval) bestEval = search.evaluation;
			alpha = max(alpha, bestEval);
		}
		else {
			if (search.evaluation < bestEval) bestEval = search.evaluation;
			beta = min(beta, bestEval);
		}

		if (beta <= alpha) break;
	}

	//if (depth == *n) moves = legalmoves;
	return { bestEval, moves };
}

inline Bot_SB::search_result Bot_SB::SearchCaptures(int depth, int alpha, int beta, bool maximizingPlayer) {
	int bestEval = (maximizingPlayer) ? INIT_MAX : INIT_MIN;
	bool color = (maximizingPlayer) ? WHITE : BLACK;
	vector<Move> moves;
	if (depth == 0) {
		int eval = (EvaluateGame(color) - EvaluateGame(!color));
		if (board->scanCheckMate(!color)) {
			eval = (maximizingPlayer) ? 1000000 : -1000000;
		}
		return { eval, };
	}
	if (board->scanCheckMate(color)) {
		int eval = (maximizingPlayer) ? -1000000 : +1000000;
		return { eval, };
	}

	///BUSQUEDA DE MOVIMIENTOS DE CAPTURA///
	for (Move& move : generateCaptureMoves(color)) {
		MoveResult result = board->makeMove(move);
		if (!result.success) continue;

		search_result search = SearchCaptures(depth - 1, alpha, beta, !maximizingPlayer);
		move.score = search.evaluation;
		moves.push_back(move);
		board->undoMove();

		if (maximizingPlayer) {
			if (search.evaluation > bestEval) bestEval = search.evaluation;
			alpha = max(alpha, bestEval);
		}
		else {
			if (search.evaluation < bestEval) bestEval = search.evaluation;
			beta = min(beta, bestEval);
		}

		if (beta <= alpha) break;
	}
	return { bestEval, moves };
}

inline Move Bot_SB::botMove(bool turn) {

	auto start = chrono::high_resolution_clock::now();

	int depth = 4;
	if (board->countBits(board->getPieces(WHITE | BLACK)) < 15) depth = 5;

	search_result search = SearchMoves(depth, INIT_MAX, INIT_MIN, (turn == WHITE));


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
		auto end = chrono::high_resolution_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(end - start).count() > 2000) {
			cout << "\nTiempo: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms";
			break;
		}
	}*/


	if (search.moves.empty()) {
		// Si no hay movimientos, devolver un movimiento nulo o lanzar error
		cout << "\n no hay ningun movimiento legal detectado";
		Move defaultMove = { 0, 0, NONE, DEFAULT };
		return defaultMove;
	}

	sortMovesByEvaluation(search.moves, turn);

	return  search.moves[0];
}

inline void Bot_SB::sortMovesByEvaluation(vector<Move>& moves, bool color) {
	if (color == WHITE) { // Bot es blanco: maximiza (orden descendente)
		sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
			return a.score > b.score;
			});
	}
	else { // Bot es negro: minimiza (orden ascendente)
		sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
			return a.score < b.score;
			});
	}
}