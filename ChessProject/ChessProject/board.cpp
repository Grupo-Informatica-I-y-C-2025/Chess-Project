#pragma once
#include "board.h"



Bitboard Board::generateAttacksFrom(int sq, bool color) const {
	int pieceType = BitboardGetType(sq);
	switch (pieceType) {
	case PAWN: return pawnAttacks[color][sq] ;
	case KNIGHT: return knightMoves[sq];
	case BISHOP: return generateBishopMoves(sq, color);
	case ROOK: return generateRookMoves(sq, color);
	case QUEEN: return generateQueenMoves(sq, color);
	case KING: return kingMoves[sq];
	default: return 0;
	}
}
Bitboard Board::calculateAttackMap(bool color) const {
	Bitboard attacks = 0;
	Bitboard pieces = getPieces(color);

	while (pieces) {
		int sq = portable_ctzll(pieces);
		attacks |= generateAttacksFrom(sq, color);
		pieces &= pieces - 1;
	}
	return attacks;
}


Bitboard Board::generateMovesFrom(int sq, bool color) const {
	int pieceType = BitboardGetType(sq);
	switch (pieceType) {
	case PAWN: return generatePawnMoves(sq, color);
	case KNIGHT: return generateKnightMoves(sq, color);
	case BISHOP: return generateBishopMoves(sq, color);
	case ROOK: return generateRookMoves(sq, color);
	case QUEEN: return generateQueenMoves(sq, color);
	case KING: return generateKingMoves(sq, color);
	default: return 0;
	}
}
Bitboard Board::getMovementMap(bool color)const {
	Bitboard movementMap = 0;
	for (int pt = PAWN; pt <= KING; pt++) {
		Bitboard pieces = currentState.pieces[color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			movementMap |= generateMovesFrom(sq, color);
			pieces &= pieces - 1;
		}
	}
	return movementMap & ~getPieces(color); // Excluye casillas ocupadas por propias
}

vector<Move> Board::generateAllMoves(bool color) {
	vector<Move> allMoves;
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return allMoves;

	for (int source = 0; source < 64; ++source) {
		if (BitboardGetColor(source) != color)continue;
		int pieceType = BitboardGetType(source);

		Bitboard movesBB = generateMovesFrom(source,color);

		while (movesBB) {
			int target = portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			if (isLegalmove(move)) {
				if(move.moveType==PROMOTION){
					if (getVariant()==0) {
						move.promotionPiece = QUEEN;
						allMoves.push_back(move);
						move.promotionPiece = KNIGHT;
						allMoves.push_back(move);
						move.promotionPiece = BISHOP;
						allMoves.push_back(move);
						move.promotionPiece = ROOK;
						allMoves.push_back(move);
					}
					if (getVariant() == 1) {
						move.promotionPiece = KNIGHT;
						allMoves.push_back(move);
						move.promotionPiece = BISHOP;
						allMoves.push_back(move);
						move.promotionPiece = ROOK;
						allMoves.push_back(move);
					}
					if (getVariant() == 2) {
						move.promotionPiece = QUEEN;
						allMoves.push_back(move);
						move.promotionPiece = ROOK;
						allMoves.push_back(move);
					}
				}
				else allMoves.push_back(move);
			}
		}
	}
	return allMoves;
}
vector<Move> Board::generateCaptureMoves(bool color) {
	vector<Move> allMoves;
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return allMoves;
	Bitboard enemyPieces = getPieces(!color) | currentState.enPassant;

	for (int source = 0; source < 64; ++source) {
		if (BitboardGetColor(source) != color)continue;
		int pieceType = BitboardGetType(source);

		Bitboard movesBB = generateAttacksFrom(source, color);
		movesBB &= enemyPieces;

		while (movesBB) {
			int target = portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			if (isLegalmove(move))allMoves.push_back(move);
		}
	}
	return allMoves;
}
vector<Move> Board::generateQuiescientMoves(bool color) {
	vector<Move> allMoves;
	for  (auto& const move : generateAllMoves(color)) {
		if (move.ischeck)allMoves.push_back(move);
		if (move.moveType == CAPTURE || move.moveType == EN_PASSANT || move.moveType == PROMOTION)allMoves.push_back(move);
		
	}
	return allMoves;
}
vector<Move> Board::generateCheckMoves(bool color) {
	vector<Move> allMoves;
	for (auto& const move : generateAllMoves(color)) {
		if (move.ischeck)allMoves.push_back(move);
	}
	return allMoves;
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
bool Board::isLegalmove(Move& move) {

	bool color = currentState.turn;
	int targetColor = BitboardGetColor(move.targetSquare);
	Bitboard sourceBB = 1ULL << move.sourceSquare;
	Bitboard targetBB = 1ULL << move.targetSquare;
	//casilla vacia
	if (!(currentState.pieces[color][move.pieceType] & sourceBB)) {
		return false;
	}
	//kramnik
	if (targetColor == color) {
		if(!isKramnik())return false;
	}
	//destino fuera de tablero (no 8*8)
	if (targetBB & boundary) return false;
	Bitboard movesBB = generateMovesFrom(move.sourceSquare, color);
	if (!isKramnik())movesBB & ~getPieces(color);

	if (!(movesBB & (1ULL << move.targetSquare)))return false;

	MoveResult result = makeMove(move);
	if (!result.success) return false;
	move.ischeck = scanChecks(!color);
	bool inCheck = scanChecks(color);
	undoMove();
	
	return inCheck ? false : true;
}
MoveType Board::determineMoveType(const Move& move) const {
	const bool color = currentState.turn ? BLACK : WHITE;
	const Bitboard targetBB = 1ULL << move.targetSquare;

	if (move.pieceType == PAWN) {
		if (getVariant()==2) {
			const int promotionRank = (color == WHITE) ? 4 : 0;
			if (move.targetSquare / 8 == promotionRank) {

				return PROMOTION;
			}
		}
		else {
			const int promotionRank = (color == WHITE) ? 7 : 0;
			if (move.targetSquare / 8 == promotionRank) {

				return PROMOTION;
			}
		}
	}

	// 2. Verificar enroque
	if (move.pieceType == KING) {
		const int delta = abs(move.sourceSquare - move.targetSquare);
		if (delta == 2) { // Movimiento de 2 casillas (kingside/queenside)
			
			return CASTLING;
		}
	}

	// 3. Verificar al paso
	if (move.pieceType == PAWN && (targetBB & currentState.enPassant)) {
		
		return EN_PASSANT;
	}
	
	// 4. Verificar captura normal
	if (getPieces(!color) & targetBB) {
		
		return CAPTURE;
	}

	return DEFAULT;
}
MoveResult Board::makeMove( Move& move) {
	MoveResult result;
	result.success = false;

	// Guardar estado previo
	result.oldEnPassant = currentState.enPassant;
	result.oldCastlingRights = currentState.castlingRights;

	// Resetear en passant
	currentState.enPassant = 0;

	// Guardar captura
	int capturedPiece = BitboardGetType(move.targetSquare);
	int capturedColor = BitboardGetColor(move.targetSquare);

	if (capturedPiece != NONE) {
		result.capturedPiece = capturedPiece;
		result.capturedColor = capturedColor;
	}
	move.moveType = determineMoveType(move);
	if (move.moveType == PROMOTION) {
		bool valid = false;
		switch (getVariant()) {
		case 0:
			valid = (move.promotionPiece == QUEEN || move.promotionPiece == ROOK
				|| move.promotionPiece == BISHOP || move.promotionPiece == KNIGHT);
			break;
		case 1:
			valid = (move.promotionPiece == ROOK || move.promotionPiece == BISHOP
				|| move.promotionPiece == KNIGHT);
			break;
		case 2:
			valid = (move.promotionPiece == QUEEN || move.promotionPiece == ROOK);
			break;
		}
		if (!valid) {
			result.success = false;
			return result;
		}
	}
	// 1. Obtener máscaras de bits
	Bitboard source = 1ULL << move.sourceSquare;
	Bitboard target = 1ULL << move.targetSquare;
	bool color = currentState.turn ? BLACK : WHITE;

	int targetType = BitboardGetType(move.targetSquare);
	int targetColor = BitboardGetColor(move.targetSquare);

	// 3. Guardar estado actual
	if (moveCount >= 500) {
		return result;
	}
	prevStates[moveCount] = currentState;
	moveCount++;
	// 4. Realizar movimiento base
	currentState.pieces[color][move.pieceType] ^= source | target;
	currentState.occupancy ^= source | target;

	// 5. Manejar casos especiales
	result.capturedPiece = NONE;
	if (move.moveType == EN_PASSANT) {
		int epSquare = move.targetSquare + (color == WHITE ? -8 : 8);
		result.capturedPiece = PAWN;
		result.capturedColor = !color;
		currentState.pieces[!color][PAWN] ^= (1ULL << epSquare);
		currentState.occupancy ^= (1ULL << epSquare);
	}
	else if (move.moveType == CASTLING) {

		Bitboard castlingType;
		int delta = (move.targetSquare - move.sourceSquare);

		if ((currentState.castlingRights & WHITE_QUEENSIDE) && delta == -2 && move.sourceSquare == 4) {
			currentState.pieces[WHITE][ROOK] ^= 1ULL << 3;
			currentState.pieces[WHITE][ROOK] ^= 1ULL << 0;
		}else if ((currentState.castlingRights & WHITE_KINGSIDE) && delta == 2 && move.sourceSquare == 4) {
			currentState.pieces[WHITE][ROOK] ^= 1ULL << 5;
			currentState.pieces[WHITE][ROOK] ^= 1ULL << 7;
		}else if ((currentState.castlingRights & WHITE_QUEENSIDE) && delta == -2 && move.sourceSquare == 60) {
			currentState.pieces[BLACK][ROOK] ^= 1ULL << 59;
			currentState.pieces[BLACK][ROOK] ^= 1ULL << 56;
		}else if ((currentState.castlingRights & WHITE_KINGSIDE) && delta == 2 && move.sourceSquare == 60) {
			currentState.pieces[BLACK][ROOK] ^= 1ULL << 61;
			currentState.pieces[BLACK][ROOK] ^= 1ULL << 63;
		}
	}
	else if (move.pieceType == KING) {
		currentState.castlingRights &= ~(color ? (BLACK_KINGSIDE | BLACK_QUEENSIDE) : (WHITE_KINGSIDE | WHITE_QUEENSIDE));
	}
	else if (move.pieceType == ROOK) {
		// Check if the rook is on its initial square
		if (color == WHITE) {
			if (move.sourceSquare == 0) currentState.castlingRights &= ~WHITE_QUEENSIDE;
			else if (move.sourceSquare == 7) currentState.castlingRights &= ~WHITE_KINGSIDE;
		}
		else {
			if (move.sourceSquare == 56) currentState.castlingRights &= ~BLACK_QUEENSIDE;
			else if (move.sourceSquare == 63) currentState.castlingRights &= ~BLACK_KINGSIDE;
		}
	}
	else if (move.moveType == PROMOTION) {
		currentState.pieces[color][PAWN] ^= target;     // Eliminar peón
		currentState.pieces[color][move.promotionPiece] ^= target;    // Añadir reina
	}
	// Captura normal
	if (targetType != 6 && targetColor !=2) {
		if (isKramnik() || targetColor != color) {
			currentState.pieces[targetColor][targetType] ^= target;
			result.capturedPiece = targetType;
			result.capturedColor = targetColor;
		}
	}

	// 6. Actualizar estado del juego
	currentState.occupancy = 0;
	for (int i = PAWN; i <= KING; i++) {
		currentState.occupancy |= currentState.pieces[color][i] | currentState.pieces[!color][i];
	}
	updateAttackMap();
	currentState.enPassant = (move.pieceType == PAWN && abs(move.targetSquare - move.sourceSquare) == 16)
		? color? (1ULL << (move.sourceSquare - 8)): (1ULL << (move.sourceSquare + 8)) : 0;

	currentState.turn = !currentState.turn;
	result.success = true;
	return result;
}
void Board::undoMove() {
	if (moveCount == 0) return;
	//cout << "\n move count :" << moveCount;
	moveCount--;
	currentState = prevStates[moveCount];
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
	/*// Precalcular máscaras de alfil (direcciones diagonales)
	for (int sq = 0; sq < 64; ++sq) {
		bishopMasks[sq] = 0;
		int x = sq % 8;
		int y = sq / 8;

		// Diagonales en las 4 direcciones
		for (int dx : {-1, 1}) {
			for (int dy : {-1, 1}) {
				int cx = x + dx;
				int cy = y + dy;
				while (cx >= 0 && cx < 8 && cy >= 0 && cy < 8) {
					bishopMasks[sq] |= 1ULL << (cy * 8 + cx);
					cx += dx;
					cy += dy;
				}
			}
		}
	}

	// Precalcular máscaras de torre (filas y columnas)
	for (int sq = 0; sq < 64; ++sq) {
		rookMasks[sq] = 0;
		int x = sq % 8;
		int y = sq / 8;

		// Horizontal
		for (int dx = -1; dx <= 1; dx += 2) {
			int cx = x + dx;
			while (cx >= 0 && cx < 8) {
				rookMasks[sq] |= 1ULL << (y * 8 + cx);
				cx += dx;
			}
		}

		// Vertical
		for (int dy = -1; dy <= 1; dy += 2) {
			int cy = y + dy;
			while (cy >= 0 && cy < 8) {
				rookMasks[sq] |= 1ULL << (cy * 8 + x);
				cy += dy;
			}
		}
	}*/
}

void Board::initPositionalMasks() {
	// Precalcular máscaras para peones pasados
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

Bitboard Board::generatePawnMoves(int sq, bool color) const {
	Bitboard moves = 0;
	const int x = sq % 8;
	const int y = sq / 8;

	const Bitboard pawn = 1ULL << sq;
	const Bitboard enemies = getPieces(!color) | currentState.enPassant;
	const Bitboard empty = ~currentState.occupancy;

	const int forward = (color == WHITE) ? 8 : -8;
	Bitboard singlePush = shift(pawn, forward) & empty;
	moves |= singlePush;

	if (getVariant()!=2) {
		if ((color == WHITE && y == 1) || (color == BLACK && y == 6)) {
			Bitboard doublePush = shift(singlePush, forward) & empty;
			moves |= doublePush;
		}
	}
	// Capturas usando máscaras precalculadas
	moves |= pawnAttacks[color][sq] & enemies;

	return moves;
}
Bitboard Board::generateRookMoves(int sq, bool color) const {
	Bitboard occ = currentState.occupancy;
	Bitboard mask = rookMasks[sq];
	Bitboard block = occ & mask;
	int      bits = rookIndexBits[sq];            // dynamic occupancy bits
	uint64_t magic = rookMagics[sq];
	uint64_t idx = (block * magic) >> (64 - bits);
	// guard against out-of-bounds
	if (idx >= (1u << bits)) return 0ULL;
	return rookAttacks[sq][idx];
}

Bitboard Board::generateBishopMoves(int sq, bool color) const {
	Bitboard occ = currentState.occupancy;
	Bitboard mask = bishopMasks[sq];
	Bitboard block = occ & mask;
	int      bits = bishopIndexBits[sq];         // dynamic occupancy bits
	uint64_t magic = bishopMagics[sq];
	uint64_t idx = (block * magic) >> (64 - bits);
	if (idx >= (1u << bits)) return 0ULL;
	return bishopAttacks[sq][idx];
}
Bitboard Board::generateKnightMoves(int sq, bool color) const {
    Bitboard moves = knightMoves[sq];
    return moves;
}
Bitboard Board::generateQueenMoves(int sq, bool color) const {
	return generateRookMoves(sq, color) | generateBishopMoves(sq, color);
}
Bitboard Board::generateKingMoves(int sq, bool color) const {
	Bitboard moves = kingMoves[sq]| generateCastlingMoves(color);
	Bitboard enemyAttacks = getAttackMap(!color);
	return moves & ~enemyAttacks;
}
Bitboard Board::generateCastlingMoves(bool color) const {
	Bitboard castling = 0;
	if (!currentState.castlingRights)return castling;
	const Bitboard occupancy = currentState.occupancy;
	const Bitboard attacks = getAttackMap(!color);

	// Kingside castling
	if (currentState.castlingRights & (color ? BLACK_KINGSIDE : WHITE_KINGSIDE)) {
		const Bitboard path = color == WHITE ? 0x60ULL : 0x6000000000000000ULL;
		if (!(occupancy & path) && !(attacks & (path >> 1))) {
			castling |= color == WHITE ? 0x40ULL : 0x4000000000000000ULL;
		}
	}

	// Queenside castling
	if (currentState.castlingRights & (color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE)) {
		const Bitboard path = color == WHITE ? 0xEULL : 0xE00000000000000ULL;
		if (!(occupancy & path) && !(attacks & (path << 1))) {
			castling |= color == WHITE ? 0x4ULL : 0x400000000000000ULL;
		}
	}

	return castling;
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
			Bitboard attacks = generateAttacksFrom(sq, !color);

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
bool Board::canKingMove(bool color) {
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
bool Board::canCaptureAttacker(bool color, Bitboard attackerBB) {
	int attackerSquare = portable_ctzll(attackerBB);
	int kingSquare = portable_ctzll(currentState.pieces[color][KING]);

	Bitboard myAttackMap = getAttackMap(color);
	if (!(myAttackMap & attackerBB)) return false;

	for (int type = PAWN; type <= KING; type++) {
		Bitboard pieces = currentState.pieces[color][type];
		while (pieces) {
			int capturer = portable_ctzll(pieces);
			pieces &= pieces - 1;

			Bitboard attacksBB = generateAttacksFrom(capturer, color);

			//si la pieza no ataca a attackers, omite el retso del bucle
			if (!(attacksBB & attackerBB))continue;


			if (!checkRays(capturer, attackerSquare, kingSquare, color))continue;
			return true;
		}
	}
	return false;
}
bool Board::canBlockAttacker(bool color, Bitboard attackerBB) {
	int kingSquare = portable_ctzll(currentState.pieces[color][KING]);
	int attackerSquare = portable_ctzll(attackerBB);
	int attackerType = BitboardGetType(attackerSquare);

	if (attackerType != BISHOP && attackerType != ROOK && attackerType != QUEEN) return false;

	Bitboard path = getPath(kingSquare, attackerSquare, attackerType);
	Bitboard myMovementMap = getMovementMap(color);
	Bitboard blockingSquares = path & myMovementMap;
	if (!blockingSquares)return false;




	while (blockingSquares) {
		int blockSq = portable_ctzll(blockingSquares);

		for (int type = PAWN; type <= KING; type++) {
			Bitboard pieces = currentState.pieces[color][type];
			while (pieces) {
				int blocker = portable_ctzll(pieces);
				pieces &= pieces - 1;
				Bitboard MovesBB = generateMovesFrom(blocker, color);

				//si la pieza no ataca a blocksq, omite el retso del bucle
				if (!(MovesBB & (1ULL<<blockSq)))continue;

				if (checkRays(blocker,blockSq,kingSquare, color))return true;

			}
		}
	blockingSquares &= blockingSquares - 1;
	}
	return false;
}

Bitboard Board::getPath(int kingSquare, int attackerSquare, int attackerType) {
	int dx = abs((kingSquare % 8) - (attackerSquare % 8));
	int dy = abs((kingSquare / 8) - (attackerSquare / 8));

	// Verificar alineación según el tipo de pieza
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
//añadir en passant
bool Board::checkRays(int source, int target, int King, bool color) {
	Bitboard newOccupancy = currentState.occupancy;
	newOccupancy &= ~(1ULL << source);  // Eliminar la pieza de origen

	// Si la pieza que se mueve es el rey, actualizar su posición
	bool isKingMove = (source == King);
	if (isKingMove) {
		newOccupancy &= ~(1ULL << source); // Eliminar posición original del rey
		newOccupancy |= (1ULL << target);   // Añadir nueva posición
		King = target; // Actualizar la posición del rey para el cálculo
	}
	else {
		newOccupancy |= (1ULL << target);   // Añadir pieza al destino
	}

	// Verificar ataques de piezas alineadas (BISHOP, ROOK, QUEEN)
	for (int piece = BISHOP; piece <= QUEEN; piece++) {
		Bitboard rays = currentState.pieces[!color][piece];
		while (rays) {
			int attackerSq = portable_ctzll(rays);
			rays &= rays - 1;

			if (attackerSq == target) continue; // La pieza movida puede ser capturada

			Bitboard path = getPath(King, attackerSq, piece);
			if (!path) continue; // No está alineado

			// Verificar si el camino está bloqueado en el nuevo tablero
			if ((path & newOccupancy) == 0) {
				return false; // Camino despejado: jaque no bloqueado
			}
		}
	}
	return true; // No hay jaques por piezas alineadas
}


bool Board::scanCheckMate(bool color) {
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

bool Board::scanRepetition() {
	// Check if the current position has been seen before
	int count = 0;
	for (int i = 0; i < moveCount; ++i) {
		if (prevStates[i] == currentState) {
			count++;
		}
	}
	return (count>2);
}