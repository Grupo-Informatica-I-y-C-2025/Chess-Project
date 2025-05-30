#include "Bot_V4.h"
#include "generator.h"
#include "evaluation.h"
#include "openings.h"
#include <random>
#include <chrono>

const int QUIESCE_DEPTH = 3;


static int totalNodes;
Bot_V4::search_result Bot_V4::SearchMoves(int depth, int alpha, int beta, Board& board) {
    totalNodes++;

    // 1. Caso base: búsqueda quiescente
    if (depth == 0) {
        return SearchQuiescent(alpha, beta, board);
    }

    // 2. Null Move Pruning (solo si no hay jaque)
    if (depth >= 2 && !board.scanChecks(board.currentState.turn)) {
        // Guardar estado completo
        ChessState prevState = board.currentState;
        
        // Simular movimiento nulo
        board.currentState.turn = !board.currentState.turn;
        Generator::updateAttackMap(board); // Actualizar ataques

        // Búsqueda reducida (evitar profundidad negativa)
        int R = 2;
        int reducedDepth = std::max(0, depth - 1 - R); // ¡Importante!
        int nullEval = -SearchMoves(reducedDepth, -beta, -beta + 1, board).evaluation;

        // Restaurar estado completo
        board.currentState = prevState;
        Generator::updateAttackMap(board);

        // Poda si es posible
        if (nullEval >= beta) {
            return {beta, {}};
        }
    }

    // 3. Búsqueda estándar Negamax/PVS
    int bestEval = INIT_MIN;
    Move bestMove = {0, 0, NONE, DEFAULT};
    vector<Move> moves = Generator::generateAllMoves(board.currentState.turn, board);
    Generator::orderMoves(moves, board);

    for (Move& move : moves) {
        MoveResult result = Generator::makeMove(move, board);
        if (!result.success) continue;

        search_result sr = SearchMoves(depth - 1, -beta, -alpha, board);
        int currentEval = -sr.evaluation;

        Generator::undoMove(board);

        if (currentEval > bestEval) {
            bestEval = currentEval;
            bestMove = move;
            alpha = std::max(alpha, bestEval);
        }

        if (alpha >= beta) break; // Corte beta
    }

    return {bestEval, bestMove};
}

Bot_V4::search_result Bot_V4::SearchQuiescent(int alpha, int beta, Board& board) {
    totalNodes++;

    // Evaluación estándar (stand-pat)
    int standPat = Evaluation::EvaluateGame(board);
    int bestEval = standPat;

    // Corte beta
    if (standPat >= beta) return {standPat, {}};

    // Actualizar alpha
    alpha = std::max(alpha, standPat);

    // Generar movimientos quiescentes (capturas y jaques)
    vector<Move> moves = Generator::generateQuiescientMoves(board.currentState.turn, board);
    Generator::orderMoves(moves, board);

    // Procesar movimientos inestables
    for (Move& move : moves) {
        MoveResult result = Generator::makeMove(move, board);
        if (!result.success) continue;

        // Llamada recursiva con Negamax
        search_result sr = -SearchQuiescent(-beta, -alpha, board);

        Generator::undoMove(board);

        // Actualizar mejor evaluación
        if (sr.evaluation > bestEval) {
            bestEval = sr.evaluation;
            alpha = std::max(alpha, bestEval);
        }

        // Corte alfa-beta
        if (alpha >= beta) break;
    }

    return {bestEval, {}};
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


Move Bot_V4::botMove(bool turn,int maxDepth,Board& board) {
	totalNodes = 0;
	
	static OpeningBook openingBook;
    std::vector<std::string> moveHistory = getMoveHistory(board); // Implementar esta función

    /*// 1. Intentar seguir una apertura
    std::string recommendedMove = openingBook.getNextMove(moveHistory);
    if (!recommendedMove.empty()) {
        Move openingMove = uciToMove(recommendedMove, board); // Convertir UCI a Move
		cout<<"opening move fond!"<<endl;
        if (Generator::isLegalmove(openingMove, board)) {
            return openingMove;
        }
    }*/

	Move bestMove;
	search_result sr;
	for (int d = 1; d <= maxDepth; ++d) {
		cout << "\n--- Search Depth " << d << " ---" << endl;
		sr = SearchMoves(d, INIT_MIN, INIT_MAX,board);
		cout << "BOT4 Evaluación: " << sr.evaluation << endl;
		cout << "BOT4 Nodos: " << totalNodes << endl;
		bestMove = sr.bestMove;
	}
	return bestMove;
}


