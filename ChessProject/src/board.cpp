
#include "board.h"
#include "generator.h"


void Board::initCached(){
	for(int i=0;i<64;i++){
		currentState.pieceType[i]=BitboardGetType(i);
		currentState.pieceColor[i]=BitboardGetColor(i);
	}
}

void Board::initAttackTables() {

	initBishopMagics();
	initRookMagics();

	initPositionalMasks();
	initializePassedPawnMasks();
	boundaryMask();
	// Precalcular ataques de peones
	for (int sq = 0; sq < 64; ++sq) {
		int x = sq % 8;
		int y = sq / 8;

		// Peones blancos
		pawnAttacks[WHITE][sq] = 0;
		if (x > 0 && y < 7) pawnAttacks[WHITE][sq] |= 1ULL << (sq + 7);
		if (x < 7 && y < 7) pawnAttacks[WHITE][sq] |= 1ULL << (sq + 9);

		// Peones negros
		pawnAttacks[BLACK][sq] = 0;
		if (x > 0 && y > 0) pawnAttacks[BLACK][sq] |= 1ULL << (sq - 9);
		if (x < 7 && y > 0) pawnAttacks[BLACK][sq] |= 1ULL << (sq - 7);
	}

	// Precalcular movimientos de caballo
	const int knightOffsets[8] = { -17, -15, -10, -6, 6, 10, 15, 17 };
	for (int sq = 0; sq < 64; ++sq) {
		knightMoves[sq] = 0;
		int x = sq % 8;
		int y = sq / 8;

		for (int offset : knightOffsets) {
			int target = sq + offset;
			if (target < 0 || target >= 64) continue;

			int tx = target % 8;
			int ty = target / 8;

			// Validar que el movimiento es en L
			int dx = abs(x - tx);
			int dy = abs(y - ty);
			if (dx + dy == 3 && dx > 0 && dy > 0) {
				knightMoves[sq] |= 1ULL << target;
			}
		}
	}

	// Precalcular movimientos de rey
	const int kingOffsets[8] = { -9, -8, -7, -1, 1, 7, 8, 9 };
	for (int sq = 0; sq < 64; ++sq) {
		kingMoves[sq] = 0;
		int x = sq % 8;
		int y = sq / 8;

		for (int offset : kingOffsets) {
			int target = sq + offset;
			if (target < 0 || target >= 64) continue;

			int tx = target % 8;
			int ty = target / 8;

			// Validar movimiento de 1 casilla
			if (abs(x - tx) <= 1 && abs(y - ty) <= 1) {
				kingMoves[sq] |= 1ULL << target;
			}
		}
	}
	Generator::updateAttackMap(*this);
}


void Board::boundaryMask() {
	Bitboard mask = 0;
	for (int i = N; i < 8; i++) {
		mask |= RANK_MASKS[i];
	}
	for (int i = M; i < 8; i++) {
		mask |= FILE_MASKS[i];
	}
	boundary = mask;
}
void Board::initPositionalMasks() {
	// Precalcular m�scaras para peones pasados
	for (int color = WHITE; color <= BLACK; ++color) {
		for (int sq = 0; sq < 64; ++sq) {
			int file = sq % 8;
			PASSED_PAWN_MASKS[color][sq] = (FILE_MASKS[file] | FILE_MASKS[max(0, file - 1)] | FILE_MASKS[min(7, file + 1)])
				& (color == WHITE ? ~RANK_MASKS[sq / 8] : RANK_MASKS[sq / 8]);
		}
	}
}
void Board::initializePassedPawnMasks() {
	for (int sq = 0; sq < 64; ++sq) {
		int file = sq % 8;
		int rank = sq / 8;

		// Blancas: avanzan hacia filas superiores (rank++)
		Bitboard whiteMask = 0ULL;
		for (int r = rank + 1; r <= 7; ++r) { // filas delante
			for (int f = max(0, file - 1); f <= min(7, file + 1); ++f) { // misma columna y adyacentes
				whiteMask |= (1ULL << (r * 8 + f));
			}
		}
		PASSED_PAWN_MASKS[WHITE][sq] = whiteMask;

		// Negras: avanzan hacia filas inferiores (rank--)
		Bitboard blackMask = 0ULL;
		for (int r = rank - 1; r >= 0; --r) { // filas delante hacia abajo
			for (int f = max(0, file - 1); f <= min(7, file + 1); ++f) {
				blackMask |= (1ULL << (r * 8 + f));
			}
		}
		PASSED_PAWN_MASKS[BLACK][sq] = blackMask;
	}
}


Bitboard Board::getRookMask(int sq)const {
	int r = sq >> 3, f = sq & 7;
	Bitboard mask = 0ULL;
	// Up
	for (int rr = r + 1; rr < 7; ++rr)
		mask |= (1ULL << (rr * 8 + f));
	// Down
	for (int rr = r - 1; rr > 0; --rr)
		mask |= (1ULL << (rr * 8 + f));
	// Right
	for (int ff = f + 1; ff < 7; ++ff)
		mask |= (1ULL << (r * 8 + ff));
	// Left
	for (int ff = f - 1; ff > 0; --ff)
		mask |= (1ULL << (r * 8 + ff));
	return mask;
}
Bitboard Board::getRookAttacksSlow(int sq, Bitboard blockers)const {
	Bitboard attacks = 0;
	int x = sq % 8, y = sq / 8;

	// Direcciones: derecha, izquierda, arriba, abajo
	const int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };

	for (auto e : dirs) {
		for (int d = 1; d < 8; ++d) {
			int cx = x + e[0] * d, cy = y + e[1] * d;
			if (cx < 0 || cx > 7 || cy < 0 || cy > 7) break;

			attacks |= 1ULL << (cy * 8 + cx);
			if (blockers & (1ULL << (cy * 8 + cx))) break;
		}
	}

	return attacks;
}

Bitboard Board::getBishopMask(int sq)const {
	int r = sq >> 3, f = sq & 7;
	Bitboard mask = 0ULL;
	// Northeast
	for (int i = 1; r + i < 7 && f + i < 7; ++i)
		mask |= (1ULL << ((r + i) * 8 + (f + i)));
	// Northwest
	for (int i = 1; r + i < 7 && f - i > 0; ++i)
		mask |= (1ULL << ((r + i) * 8 + (f - i)));
	// Southeast
	for (int i = 1; r - i > 0 && f + i < 7; ++i)
		mask |= (1ULL << ((r - i) * 8 + (f + i)));
	// Southwest
	for (int i = 1; r - i > 0 && f - i > 0; ++i)
		mask |= (1ULL << ((r - i) * 8 + (f - i)));
	return mask;
}
Bitboard Board::getBishopAttacksSlow(int sq, Bitboard blockers)const {
	Bitboard attacks = 0;
	int x = sq % 8;
	int y = sq / 8;

	const int dirs[4][2] = { {1,1}, {-1,1}, {1,-1}, {-1,-1} };

	for (auto e : dirs) {
		int cx = x + e[0];
		int cy = y + e[1];
		while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
			Bitboard pos = 1ULL << (cy * 8 + cx);
			attacks |= pos;
			if (blockers & pos) break;
			cx += e[0];
			cy += e[1];
		}
	}
	return attacks;
}



bool Board::scanChecks(bool color, Bitboard& attackers)const {
	attackers = 0;
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return true;


	// Calcular ataques enemigos al cuadrado del rey
	Bitboard attackersBB = getAttackMap(!color) & kingBB;

	if (!attackersBB) return false;

	// Encontrar piezas atacantes usando bitboards
	for (int pt = PAWN; pt <= KING; ++pt) {
		Bitboard pieces = currentState.pieces[!color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			Bitboard attacks = Generator::generateAttacksFrom(sq, !color,*this);

			if (attacks & kingBB) {
				attackers |= (1ULL << sq);
			}
			pieces &= pieces - 1;
		}
	}

	return true;
}
bool Board::scanChecks(bool color)const {
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return true;

	// Calcular ataques enemigos al cuadrado del rey
	Bitboard attackersBB = getAttackMap(!color) & kingBB;

	if (!attackersBB) return false;

	return true;
}

//no se si las sigo necesitando
bool Board::canKingMove(bool color) const{
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return false;

	int kingSquare = portable_ctzll(kingBB);

	Bitboard kingmoves = kingMoves[kingSquare];

	kingmoves &= ~getPieces(color);

	while (kingmoves) {
		int targetSquare = portable_ctzll(kingmoves);
		kingmoves &= kingmoves - 1;

		if (targetSquare & boundary) continue;
		if ((1ULL << targetSquare) & getAttackMap(!color))continue;

		if (checkRays(kingSquare,targetSquare,targetSquare, color))return true;
	}

	return false;
}
bool Board::canCaptureAttacker(bool color, Bitboard attackerBB)const {
	int attackerSquare = portable_ctzll(attackerBB);
	int kingSquare = portable_ctzll(currentState.pieces[color][KING]);

	Bitboard myAttackMap = getAttackMap(color);
	if (!(myAttackMap & attackerBB)) return false;

	for (int type = PAWN; type <= KING; type++) {
		Bitboard pieces = currentState.pieces[color][type];
		while (pieces) {
			int capturer = portable_ctzll(pieces);
			pieces &= pieces - 1;

			Bitboard attacksBB = Generator::generateAttacksFrom(capturer, color,*this);

			//si la pieza no ataca a attackers, omite el retso del bucle
			if (!(attacksBB & attackerBB))continue;


			if (!checkRays(capturer, attackerSquare, kingSquare, color))continue;
			return true;
		}
	}
	return false;
}
bool Board::canBlockAttacker(bool color, Bitboard attackerBB) const{
	int kingSquare = portable_ctzll(currentState.pieces[color][KING]);
	int attackerSquare = portable_ctzll(attackerBB);
	int attackerType = currentState.pieceType[attackerSquare];

	if (attackerType != BISHOP && attackerType != ROOK && attackerType != QUEEN) return false;

	Bitboard path = getPath(kingSquare, attackerSquare, attackerType);
	Bitboard myMovementMap = Generator::getMovementMap(color,*this);
	Bitboard blockingSquares = path & myMovementMap;
	if (!blockingSquares)return false;




	while (blockingSquares) {
		int blockSq = portable_ctzll(blockingSquares);

		for (int type = PAWN; type <= KING; type++) {
			Bitboard pieces = currentState.pieces[color][type];
			while (pieces) {
				int blocker = portable_ctzll(pieces);
				pieces &= pieces - 1;
				Bitboard MovesBB = Generator::generateMovesFrom(blocker, color,*this);

				//si la pieza no ataca a blocksq, omite el retso del bucle
				if (!(MovesBB & (1ULL<<blockSq)))continue;

				if (checkRays(blocker,blockSq,kingSquare, color))return true;

			}
		}
	blockingSquares &= blockingSquares - 1;
	}
	return false;
}

Bitboard Board::getPath(int kingSquare, int attackerSquare, int attackerType) const{
	int dx = abs((kingSquare % 8) - (attackerSquare % 8));
	int dy = abs((kingSquare / 8) - (attackerSquare / 8));

	// Verificar alineaci�n seg�n el tipo de pieza
	if (attackerType == BISHOP && dx != dy) return 0;
	if (attackerType == ROOK && dx != 0 && dy != 0) return 0;
	if (attackerType == QUEEN && (dx != dy && dx != 0 && dy != 0)) return 0;

	int stepX = (attackerSquare % 8 > kingSquare % 8) ? 1 : -1;
	int stepY = (attackerSquare / 8 > kingSquare / 8) ? 1 : -1;
	if (dx == 0) stepX = 0;
	if (dy == 0) stepY = 0;

	Bitboard path = 0;
	int current = kingSquare;
	while (true) {
		current += stepY * 8 + stepX;
		if (current == attackerSquare) break;
		if (current < 0 || current >= 64) return 0;
		path |= 1ULL << current;
	}
	return path;
}
//a�adir en passant
bool Board::checkRays(int source, int target, int King, bool color)const {
	Bitboard newOccupancy = currentState.occupancy;
	newOccupancy &= ~(1ULL << source);  // Eliminar la pieza de origen

	// Si la pieza que se mueve es el rey, actualizar su posici�n
	bool isKingMove = (source == King);
	if (isKingMove) {
		newOccupancy &= ~(1ULL << source); // Eliminar posici�n original del rey
		newOccupancy |= (1ULL << target);   // A�adir nueva posici�n
		King = target; // Actualizar la posici�n del rey para el c�lculo
	}
	else {
		newOccupancy |= (1ULL << target);   // A�adir pieza al destino
	}

	// Verificar ataques de piezas alineadas (BISHOP, ROOK, QUEEN)
	for (int piece = BISHOP; piece <= QUEEN; piece++) {
		Bitboard rays = currentState.pieces[!color][piece];
		while (rays) {
			int attackerSq = portable_ctzll(rays);
			rays &= rays - 1;

			if (attackerSq == target) continue; // La pieza movida puede ser capturada

			Bitboard path = getPath(King, attackerSq, piece);
			if (!path) continue; // No est� alineado

			// Verificar si el camino est� bloqueado en el nuevo tablero
			if ((path & newOccupancy) == 0) {
				return false; // Camino despejado: jaque no bloqueado
			}
		}
	}
	return true; // No hay jaques por piezas alineadas
}


bool Board::scanCheckMate(bool color) const{
	Bitboard attackers;
	bool inCheck = scanChecks(color, attackers);
	//cout << "\nin check?";
	if (!inCheck) return false; // Not in check, so not checkmate

	//cout << "\ndouble attack?";
	// If there are multiple attackers, only king moves can help
	if (countBits(attackers) > 1) {
		return !canKingMove(color);
	}
	//cout << "\nking can move?";
	// Check if king can move out of check
	if (canKingMove(color)) {
		return false;
	}
	//cout << "\ncan capture attacker?";
	// Check if attacker can be captured
	if (canCaptureAttacker(color, attackers)) {
		return false;
	}

	//cout << "\ncan block attackers?";
	// Check if attack can be blocked (only for sliding pieces)
	if (canBlockAttacker(color, attackers)) {
		return false;
	}


	// If none of the above, it's checkmate
	return true;
}

bool Board::scanRepetition()const {
	// Check if the current position has been seen before
	int count = 0;
	for (int i = 0; i < moveCount; ++i) {
		if (prevStates[i] == currentState) {
			count++;
		}
	}
	return (count>2);
}

