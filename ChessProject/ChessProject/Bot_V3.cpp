#pragma once
#include "Bot_V3.h"
#include <random>
#include <chrono>

#define OPENING 0
#define MIDGAME 1
#define ENDGAME 2

const int INIT_MAX = -100000000; // -10^9
const int INIT_MIN = 100000000;  // 10^9

static const int PAWN_POSITION[2][64] = {
	// White (Apertura)
	{ 0,  0,  0,  0,  0,  0,  0,  0,
	 50, 50, 50, 50, 50, 50, 50, 50,
	 10, 10, 20, 30, 30, 20, 10, 10,
	  5,  5, 10, 25, 25, 10,  5,  5,
	  0,  0,  0, 20, 20,  0,  0,  0,
	  5, -5,-10,  0,  0,-10, -5,  5,
	  5, 10, 10,-20,-20, 10, 10,  5,
	  0,  0,  0,  0,  0,  0,  0,  0 },
	  // Black (simétrico)
	{  0,  0,  0,  0,  0,  0,  0,  0,
	 5, 10, 10,-20,-20, 10, 10,  5,
	 5, -5,-10,  0,  0,-10, -5,  5,
	  0,  0,  0, 20, 20,  0,  0,  0,
	  5,  5, 10, 25, 25, 10,  5,  5,
	 10, 10, 20, 30, 30, 20, 10, 10,
	 50, 50, 50, 50, 50, 50, 50, 50,
	   0,  0,  0,  0,  0,  0,  0,  0 }
};

static const int KNIGHT_POSITION[2][64] = {
	{ -50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50},

	{ -50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50}

};

static const int KING_OPENING_POSITION[2][64] = {
	// Blanco (a1-h1) | Negro (a8-h8) reflejado
	{ -30, -20,  50, -10,  20, -10,  50, -30,
	-40, -30, -20, -20, -20, -20, -30, -40,
	-50, -40, -30, -30, -30, -30, -40, -50,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-30, -20,  50, -10,  20, -10,  50, -30},

	{-30, -20,  50, -10,  20, -10,  50, -30,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-60, -50, -40, -40, -40, -40, -50, -60,
	-50, -40, -30, -30, -30, -30, -40, -50,
	-40, -30, -20, -20, -20, -20, -30, -40,
	 -30, -20,  50, -10,  20, -10,  50, -30}
};

static const int BISHOP_POSITION[64] = {
	 -20, -10, -10, -10, -10, -10, -10, -20,
	-10,  15,   0,   0,   0,   0,  15, -10,
	-5,  10,  20,  10,  10,  20,  10,  -5,
	-5,   5,  15,  25,  25,  15,   5,  -5,
	-5,   5,  15,  25,  25,  15,   5,  -5,
	-5,  10,  20,  10,  10,  20,  10,  -5,
	-10,  15,   5,   0,   0,   5,  15, -10,
	-20, -10, -10, -10, -10, -10, -10, -20
};

static const int ROOK_POSITION[64] = {
	 5,  0,  0,  25,  25,  0,  0,  5,
	 5, 10, 10,  30,  30, 10, 10,  5,
	-5,  0,  0,  20,  20,  0,  0, -5,
	-5,  0,  0,  20,  20,  0,  0, -5,
	-5,  0,  0,  20,  20,  0,  0, -5,
	-5,  0,  0,  20,  20,  0,  0, -5,
	 5, 10, 10,  30,  30, 10, 10,  5,
	 5,  0,  0,  25,  25,  0,  0,  5
};

static const int QUEEN_POSITION[64] = {
		-20, -10, -10,  -5,  -5, -10, -10, -20,
		-10,   0,   5,   0,   0,   0,   0, -10,
		-10,  10,  15,  15,  15,  15,   5, -10,
		  0,   0,  15,  20,  20,  15,   0,  -5,
		 -5,   5,  15,  20,  20,  15,   5,  -5,
		-10,   0,  10,  15,  15,  10,   0, -10,
		-10,   5,   0,   0,   0,   0,   5, -10,
		-20, -10, -10,  -5,  -5, -10, -10, -20
	};

static const int QUEEN_OPENING_POSITION[2][64] = {
	{
		   0,   0,   0,   0,   0,   0,   0,   0,
		 -10,   5,  15,  20,  20,  15,   5, -10,
		 -20, -10,   5,  10,  10,   5, -10, -20,
		 -30, -20, -10,   0,   0, -10, -20, -30,
		 -50, -40, -30, -20, -20, -30, -40, -50,
		-100, -90, -80, -70, -70, -80, -90,-100,
		-150,-130,-120,-110,-110,-120,-130,-150,
		-200,-180,-160,-150,-150,-160,-180,-200
	},
	{
		-200,-180,-160,-150,-150,-160,-180,-200,
		-150,-130,-120,-110,-110,-120,-130,-150,
		-100, -90, -80, -70, -70, -80, -90,-100,
		 -50, -40, -30, -20, -20, -30, -40, -50,
		 -30, -20, -10,   0,   0, -10, -20, -30,
		 -20, -10,   5,  10,  10,   5, -10, -20,
		 -10,   5,  15,  20,  20,  15,   5, -10,
		   0,   0,   0,   0,   0,   0,   0,   0 
	}
};

int Bot_V3::piecePunctuation(int type) {
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

int Bot_V3::EvaluateGame(bool turn) {
	int score = 0;
	
	// Calcular puntuación absoluta
	score += centerControlEvaluation(turn) * 30;
	//score += mobilityEvaluation(turn) * 15;
	score -= kingSafetyEvaluation(turn) * 10;
	score += pawnStructureEvaluation(turn) * 15;
	score += materialEvaluation(turn) * 5;
	score += positionalEvaluation(turn) * 50;

	return 	(turn == WHITE) ? score : -score;;
}


int Bot_V3::materialEvaluation(bool turn) {
	int score = 0;
	Bitboard defense = board->getAttackMap(turn);
	Bitboard threats = board->getAttackMap(!turn);
	// Evaluación de material
	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & 1ULL << sq) {
			int type = board->BitboardGetType(sq);
			int color = board->BitboardGetColor(sq);
			int points = piecePunctuation(type);
			if (color == turn) {
				score += points;
				if ((1ULL << sq) & defense)score += 0.3*points;
				if ((1ULL << sq) & threats)score -= 0.5*points;
			}
			else {
				score -= points;
				if ((1ULL << sq) & defense)score -= 0.3 * points;
				if ((1ULL << sq) & threats)score += 0.5 * points;
			}
		}
	}
	return score;
}

int Bot_V3::gamePhase() {
	// Contar material para determinar fase del juego
	/*int totalMaterial = 0;
	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & (1ULL << sq)) {
			int type = board->BitboardGetType(sq);
			totalMaterial += piecePunctuation(type);
		}
	}*/
	int totalMaterial = board->countBits(board->currentState.occupancy);
	return (totalMaterial > 19) ? OPENING :
		(totalMaterial > 10) ? MIDGAME : ENDGAME;
}

int Bot_V3::positionalEvaluation(bool color) {
	int score = 0;
	int phase = gamePhase();

	for (int sq = 0; sq < 64; sq++) {
		if (board->currentState.occupancy & (1ULL << sq)) {
			int type = board->BitboardGetType(sq);
			int pieceColor = board->BitboardGetColor(sq);
			int adjustedSq = (color == WHITE) ? sq : 63 - sq;

			if (pieceColor != color) continue;

			switch (type) {
			case PAWN:
				score += PAWN_POSITION[pieceColor][sq];
				break;
			case KNIGHT:
				score += KNIGHT_POSITION[pieceColor][sq];
				score += board->countBits(board->bishopMasks[sq]) * 3;
				break;
			case KING:
				score += KING_OPENING_POSITION[pieceColor][sq] * 3;

				//enroque
				if ((adjustedSq == 2 || adjustedSq == 6) &&
					(board->currentState.castlingRights & (color == WHITE ? 0b11 : 0b1100))) {
					score += 100;
				}
				break;
			case QUEEN:
				if (phase == OPENING) {
					score += QUEEN_OPENING_POSITION[pieceColor][sq];
				}
				else {
					score += QUEEN_POSITION[sq];
				}
				break;
			case BISHOP:
				score += BISHOP_POSITION[sq];
				//bonus por movilidad
				score += board->countBits(board->bishopMasks[sq]) * 3;
				break;
			case ROOK:
				score += ROOK_POSITION[sq];
				// Bonus por columnas abiertas
				int file = sq % 8;
				if (!(board->currentState.pieces[WHITE][PAWN] |
					board->currentState.pieces[BLACK][PAWN]) & Board::FILE_MASKS[file]) {
					score += 50;
				}
				break;
			}
		}
	}
	return score;
}

//----------------------------------------------------------------
// 1. Control del Centro (Bitboard optimizado)
//----------------------------------------------------------------
int Bot_V3::centerControlEvaluation(bool color) {
	Bitboard centerPieces = board->getPieces(color) & Board::CENTER_SQUARES;
	Bitboard enemyCenter = board->getPieces(!color) & Board::CENTER_SQUARES;

	return board->countBits(centerPieces) * 3 - board->countBits(enemyCenter) * 2;
}

//----------------------------------------------------------------
// 2. Movilidad de Piezas (Usando bitboards de movimiento)
//----------------------------------------------------------------
int Bot_V3::mobilityEvaluation(bool color) {
	int mobility = 0;

	for (int pt = KNIGHT; pt <= QUEEN; ++pt) {
		Bitboard pieces = board->currentState.pieces[color][pt];
		while (pieces) {
			int sq = board->portable_ctzll(pieces);
			Bitboard moves = board->generateMovesFrom(sq, color);
			mobility += board->countBits(moves & ~board->getPieces(color));
			pieces &= pieces - 1;
		}
	}
	return mobility;
}

//----------------------------------------------------------------
// 3. Seguridad del Rey (Análisis de ataques y estructura de peones)
//----------------------------------------------------------------
int Bot_V3::kingSafetyEvaluation(bool color) {
	int kingSq = board->portable_ctzll(board->currentState.pieces[color][KING]);
	Bitboard kingZone = (color == WHITE) ? Board::KING_ZONE_WHITE : Board::KING_ZONE_BLACK;
	kingZone = board->shift(kingZone, kingSq - (color == WHITE ? 4 : 60)); // Ajustar zona al rey actual

	// Penalizar ataques enemigos en la zona del rey
	Bitboard attackers = board->calculateAttackMap(!color) & kingZone;
	int attackWeight = board->countBits(attackers) * 25;

	// Bonus por peones protectores
	Bitboard pawnShield = board->currentState.pieces[color][PAWN] & kingZone;
	int shieldBonus = board->countBits(pawnShield) * 30;
	int score = attackWeight - shieldBonus;
		if (gamePhase() == OPENING) {
			if (color == WHITE) {
				if (board->currentState.castlingRights & 0b0001) score += 40; // Enroque corto
				if (board->currentState.castlingRights & 0b0010) score += 30; // Enroque largo
			}
			else {
				if (board->currentState.castlingRights & 0b0100) score += 40;
				if (board->currentState.castlingRights & 0b1000) score += 30;
			}
		}
	return score;
}

//----------------------------------------------------------------
// 4. Estructura de Peones (Dobles, aislados, pasados)
//----------------------------------------------------------------
int Bot_V3::pawnStructureEvaluation(bool color) {
	Bitboard pawns = board->currentState.pieces[color][PAWN];
	int score = 0;

	// Peones doblados
	for (int file = 0; file < 8; ++file) {
		Bitboard filePawns = pawns & Board::FILE_MASKS[file];
		int count = board->countBits(filePawns);
		if (count > 1) score -= 30 * (count - 1);
	}

	// Peones aislados
	Bitboard isolated = 0;
	for (int file = 0; file < 8; ++file) {
		Bitboard filePawns = pawns & Board::FILE_MASKS[file];
		if (filePawns && !(pawns & (Board::FILE_MASKS[max(0, file - 1)] | Board::FILE_MASKS[min(7, file + 1)]))) {
			isolated |= filePawns;
		}
	}
	score -= 25 * board->countBits(isolated);

	// Peones pasados
	Bitboard passed = pawns;
	while (passed) {
		int sq = board->portable_ctzll(passed);
		passed &= passed - 1;
		if (!(board->currentState.pieces[!color][PAWN] & board->PASSED_PAWN_MASKS[color][sq])) {
			int rank = color == WHITE ? (sq / 8) : (7 - (sq / 8));
			score += 40 + 10 * rank; // Bonus progresivo
		}
	}

	return score;
}

//----------------------------------------------------------------
// 5. Coordinación de Piezas (Ataques en mismas áreas)
//----------------------------------------------------------------
int Bot_V3::pieceCoordinationEvaluation(bool color) {
	int coordination = 0;
	Bitboard allPieces = board->getPieces(color);

	// Alfiles en diagonales abiertas
	Bitboard bishops = board->currentState.pieces[color][BISHOP];
	while (bishops) {
		int sq = board->portable_ctzll(bishops);
		bishops &= bishops - 1;
		Bitboard diag = board->bishopMasks[sq];
		if (!(diag & allPieces)) coordination += 50;
	}

	// Torres en columnas abiertas/semiabiertas
	Bitboard rooks = board->currentState.pieces[color][ROOK];
	while (rooks) {
		int sq = board->portable_ctzll(rooks);
		rooks &= rooks - 1;
		int file = sq % 8;
		bool openFile = !(board->currentState.pieces[color][PAWN] & Board::FILE_MASKS[file]);
		bool semiOpen = !(board->currentState.pieces[!color][PAWN] & Board::FILE_MASKS[file]);

		if (openFile) coordination += 60;
		else if (semiOpen) coordination += 30;
	}

	return coordination;
}

vector<Move> Bot_V3::generateAllMoves(bool color) {
	vector<Move> legalMoves;

	legalMoves = board->generateAllMoves(color);

	orderMoves(legalMoves);
	return legalMoves;
}

vector<Move> Bot_V3::generateCaptureMoves(bool color) {
	vector<Move> legalMoves;

	legalMoves = board->generateCaptureMoves(color);

	orderMoves(legalMoves);
	return legalMoves;
}

void Bot_V3::orderMoves(vector<Move>& moves) {
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

Bot_V3::search_result Bot_V3::SearchMoves(int depth, int alpha, int beta, bool maximizingPlayer) {
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

Bot_V3::search_result Bot_V3::SearchCaptures(int depth, int alpha, int beta, bool maximizingPlayer) {
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

Move Bot_V3::botMove(bool turn) {

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

void Bot_V3::sortMovesByEvaluation(vector<Move>& moves, bool color) {
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