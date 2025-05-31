#include "board.h"
#include "evaluation.h"


// Siempre devuelve (scoreBlancas − scoreNegras)
int Evaluation::EvaluateGame(const Board& board) {
	bool color = board.currentState.turn;
	int p = evaluateColor(color, board);
	int o = evaluateColor(!color, board);
	return p - o;
}

// Evalúa todos los términos para un color concreto
int Evaluation::evaluateColor(bool color, const Board& board) {
    if (board.scanCheckMate(color)) return INIT_MIN; // Penalización más fuerte

    // Pesos ajustados por fase
    const int phase = gamePhase(board);
    const int materialWeight = (phase == ENDGAME) ? 4 : 6;
    const int safetyWeight = (phase == OPENING) ? 8 : 12;
    const int activityWeight = (phase == MIDGAME) ? 15 : 8;

    // Componentes clave
    int score = 0;
    score += materialEvaluation(color, board) * materialWeight;
    score += mobilityEvaluation(color, board) * 4;
    score += pieceActivityEvaluation(color, board) * activityWeight;
    score += pawnStructureEvaluation(color, board) * 20;
    score += centerControlEvaluation(color, board) * 35;
    score += kingSafetyEvaluation(color, board) * safetyWeight; // ¡Función modificada!
    score += positionalEvaluation(color, board); // Sin división

    // Bonus/Penalizaciones estratégicas
    score += bishopPairEvaluation(color, board) * 60;
    score += rookOpenFileEvaluation(color, board) * 25;
    score -= badBishopEvaluation(color, board) * 20;
    //score += passedPawnEvaluation(color, board) * 30; // Nueva función

    // Ajuste por ventaja de desarrollo en apertura
   /* if (phase == OPENING) {
        score += developmentBonus(color, board) * 40;
    }*/

    return score;
}

int Evaluation::materialEvaluation(bool turn, const Board& board) {
	int score = 0;
	Bitboard defense = board.getAttackMap(turn);
	Bitboard threats = board.getAttackMap(!turn);
	// Evaluación de material
	for (int sq = 0; sq < 64; sq++) {
		if (board.currentState.occupancy & 1ULL << sq) {
			int type = board.BitboardGetType(sq);
			int color = board.BitboardGetColor(sq);
			int points = board.piecePunctuation(type);
			if (color == turn) {
				score += points;
				if ((1ULL << sq) & defense)score += 0.3 * points;
				if ((1ULL << sq) & threats)score -= 0.5 * points;
			}
		}
	}
	return score;
}

int Evaluation::pieceActivityEvaluation(bool color,const Board& board) {
	int activity = 0;
	// Índices 0=PAWN,1=KNIGHT,2=BISHOP,3=ROOK,4=QUEEN,5=KING
	static const int WEIGHTS[6] = { 1, 4, 4, 2, 1, 0 };

	// Mapa de piezas propias
	Bitboard ownPieces = board.getPieces(color);
	// Mapa de ataques enemigos
	Bitboard enemyAttackMap = board.getAttackMap(!color);

	for (int pt = PAWN; pt <= QUEEN; ++pt) {
		Bitboard pieces = board.currentState.pieces[color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			pieces &= pieces - 1;

			// Movimientos legales a casillas sin piezas propias
			Bitboard moves = Generator::generateMovesFrom(sq, color,board) & ~ownPieces;
			int totalMoves = countBits(moves);

			// Movimientos "seguros": que no van a casillas bajo ataque enemigo
			int safeMoves = countBits(moves & ~enemyAttackMap);

			// Actividad = safeMoves + (totalMoves - safeMoves)/2
			activity += WEIGHTS[pt] * (safeMoves + (totalMoves - safeMoves) / 2);
		}
	}
	return activity;
}

int Evaluation::mobilityEvaluation(bool color,const Board& board) {
	int mobility = 0;
	for (int pt = KNIGHT; pt <= QUEEN; ++pt) {
		Bitboard pieces = board.currentState.pieces[color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			pieces &= pieces - 1;
			Bitboard moves = Generator::generateMovesFrom(sq, color,board) & ~board.getPieces(color);
			mobility += countBits(moves);
		}
	}
	return mobility;
}

int Evaluation::pawnStructureEvaluation(bool color,const Board& board) {
	Bitboard pawns = board.currentState.pieces[color][PAWN];
	Bitboard enemyPawns = board.currentState.pieces[!color][PAWN];
	int score = 0;

	// Dobles: penalización 50 por peón adicional en la misma columna
	for (int file = 0; file < 8; ++file) {
		Bitboard fileMask = FILE_MASKS[file] & pawns;
		int count = countBits(fileMask);
		if (count > 1) score -= 50 * (count - 1);
	}

	// Aislados: penalización 25 por cada peón aislado
	for (int file = 0; file < 8; ++file) {
		Bitboard fileMask = FILE_MASKS[file];
		Bitboard thisFile = pawns & fileMask;
		if (thisFile) {
			Bitboard adjMask = 0ULL;
			if (file > 0) adjMask |= FILE_MASKS[file - 1];
			if (file < 7) adjMask |= FILE_MASKS[file + 1];
			if (!(pawns & adjMask)) {
				score -= 25 * countBits(thisFile);
			}
		}
	}

	// Pasados: bonificación 20 + 10*avance de rango
	Bitboard passed = pawns;
	while (passed) {
		int sq = portable_ctzll(passed);
		passed &= passed - 1;
		int rank = (color == WHITE) ? (sq / 8) : (7 - (sq / 8));
		Bitboard blockMask = board.PASSED_PAWN_MASKS[color][sq];
		if (!(enemyPawns & blockMask)) {
			score += 20 + 10 * rank;
		}
	}

	return score;
}

int Evaluation::centerControlEvaluation(bool color,const Board& board) {
	// Casillas centrales: d4, e4, d5, e5
	Bitboard center = CENTER_SQUARES;
	Bitboard ownControl = board.getAttackMap(color) & center;
	Bitboard enemyControl = board.getAttackMap(!color) & center;
	int ownCount = countBits(ownControl);
	int enemyCount = countBits(enemyControl);
	// Bonificación 3 centi-puntos por control propio, penalización 2 por control enemigo
	return ownCount * 3 - enemyCount * 2;
}

int Evaluation::kingSafetyEvaluation(bool color,const Board& board) {
	// Encontrar casilla del rey
	Bitboard kingBB = board.currentState.pieces[color][KING];
	int kingSq = portable_ctzll(kingBB);

	// Definir zona alrededor del rey (casillas adyacentes y delante de peones)
	Bitboard zone = 0ULL;
	int kFile = kingSq % 8;
	int kRank = kingSq / 8;
	for (int df = -1; df <= 1; ++df) {
		for (int dr = -1; dr <= 1; ++dr) {
			if (df == 0 && dr == 0) continue;
			int f = kFile + df;
			int r = kRank + dr;
			if (f >= 0 && f < 8 && r >= 0 && r < 8) {
				zone |= 1ULL << (r * 8 + f);
			}
		}
	}

	// Conteo de ataques enemigos en zona
	Bitboard enemyAttacks = Generator::calculateAttackMap(!color,board) & zone;
	int attackCount = countBits(enemyAttacks);
	int attackWeight = attackCount * 25; // 25 centi-puntos por atacante

	// Bonus por peones del escudo del rey en zona
	Bitboard pawnShield = board.currentState.pieces[color][PAWN] & zone;
	int shieldCount = countBits(pawnShield);
	int shieldBonus = shieldCount * 30;

	return attackWeight - shieldBonus;
}

int Evaluation::bishopPairEvaluation(bool color,const Board& board) {
	int count = countBits(board.currentState.pieces[color][BISHOP]);
	return (count >= 2) ? 100 : 0;
}

int Evaluation::rookOpenFileEvaluation(bool color,const Board& board){
	int bonus = 0;
	Bitboard rooks = board.currentState.pieces[color][ROOK];
	Bitboard ownPawns = board.currentState.pieces[color][PAWN];
	Bitboard enemyPawns = board.currentState.pieces[!color][PAWN];
	while (rooks) {
		int sq = portable_ctzll(rooks);
		rooks &= rooks - 1;
		int file = sq % 8;
		Bitboard fileMask = FILE_MASKS[file];
		bool open = !(ownPawns & fileMask);
		bool semiOpen = !(enemyPawns & fileMask);
		if (open)      bonus += 50;
		else if (semiOpen) bonus += 20;
	}
	return bonus;
}

int Evaluation::badBishopEvaluation(bool color,const Board& board) {
	int penalty = 0;
	Bitboard bishops = board.currentState.pieces[color][BISHOP];
	Bitboard ownPieces = board.getPieces(color);
	while (bishops) {
		int sq = portable_ctzll(bishops);
		bishops &= bishops - 1;
		Bitboard moves = Generator::generateMovesFrom(sq, color,board) & ~ownPieces;
		if (countBits(moves) < 4) penalty += 30;
	}
	return penalty;
}

int Evaluation::seventhRankEvaluation(bool color,const Board& board) {
	int bonus = 0;
	int rank = (color == WHITE) ? 6 : 1;  // séptima fila para cada color
	Bitboard mask = RANK_MASKS[rank];
	Bitboard heavy = board.currentState.pieces[color][ROOK] |
		board.currentState.pieces[color][QUEEN];
	bonus = countBits(heavy & mask) * 25;
	return bonus;
}


int Evaluation::gamePhase(const Board& board) {
	// Contar material para determinar fase del juego
	/*int totalMaterial = 0;
	for (int sq = 0; sq < 64; sq++) {
		if (board.currentState.occupancy & (1ULL << sq)) {
			int type = board.BitboardGetType(sq);
			totalMaterial += piecePunctuation(type);
		}
	}*/
	int totalMaterial = countBits(board.currentState.occupancy);
	return (totalMaterial > 19) ? OPENING :
		(totalMaterial > 10) ? MIDGAME : ENDGAME;
}

int Evaluation::positionalEvaluation(bool color,const Board& board) {
	int score = 0;
	int phase = gamePhase(board);

	for (int sq = 0; sq < 64; sq++) {
		if (board.currentState.occupancy & (1ULL << sq)) {
			int type = board.BitboardGetType(sq);
			int pieceColor = board.BitboardGetColor(sq);
			int adjustedSq = (color == WHITE) ? sq : 63 - sq;

			if (pieceColor != color) continue;

			switch (type) {
			case PAWN:
				score += PAWN_POSITION[pieceColor][sq];
				break;
			case KNIGHT:
				score += KNIGHT_POSITION[pieceColor][sq];
				score += countBits(board.bishopMasks[sq]) * 3;
				break;
			case KING:
				score += KING_OPENING_POSITION[pieceColor][sq] * 3;

				//enroque
				if ((adjustedSq == 2 || adjustedSq == 6) &&
					(board.currentState.castlingRights & (color == WHITE ? 0b11 : 0b1100))) {
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
				score += countBits(board.bishopMasks[sq]) * 3;
				break;
			case ROOK:
				score += ROOK_POSITION[sq];
				// Bonus por columnas abiertas
				int file = sq % 8;
				if (!(board.currentState.pieces[WHITE][PAWN] |
					board.currentState.pieces[BLACK][PAWN]) & FILE_MASKS[file]) {
					score += 50;
				}
				break;
			}
		}
	}
	return score;
}



