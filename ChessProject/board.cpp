#pragma once
#include "board.h"



Bitboard Board::generateAttacksFrom(int sq, bool color) const {
	int pieceType = BitboardGetType(sq % 8, sq / 8);
	switch (pieceType) {
	case PAWN: return pawnAttacks[color][sq];
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


vector<Move> Board::generateAllMoves(bool color) {  // Removido 'const'
	vector<Move> allMoves;
	for (int source = 0; source < 64; ++source) {
		int pieceType = BitboardGetType(source % 8, source / 8);
		if (pieceType == 6) continue;

		Bitboard movesBB;

		switch (pieceType) {
		case PAWN:		movesBB = generatePawnMoves(source, color); break;
		case KNIGHT:	movesBB = generateKnightMoves(source, color); break;
		case BISHOP:	movesBB = generateBishopMoves(source, color); break;
		case ROOK:		movesBB = generateRookMoves(source, color); break;
		case QUEEN:		movesBB = generateQueenMoves(source, color); break;
		case KING:		movesBB = generateKingMoves(source, color); break;
		default:		movesBB = 0; break;
		}
		//printBoard(movesBB);
		while (movesBB) {
			int target = portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			// Aplicar movimiento temporalmente
			MoveResult result = makeMove(move);
			if (result.success) {
				// Verificar si el rey está en jaque después del movimiento
				bool inCheck = scanChecks(color);
				if (!inCheck) {
					allMoves.push_back(move);
				}
				undoMove();  // Revertir el movimiento
			}
		}
	}
	return allMoves;
}
bool Board::isLegalmove(Move& move) {
	// 1. Verificar turno y pieza en casilla origen
	int color = currentState.turn;
	Bitboard sourceBB = 1ULL << move.sourceSquare;
	if (!(currentState.pieces[color][move.pieceType] & sourceBB)) {
		return false;
	}

	int pieceType = BitboardGetType(move.sourceSquare % 8, move.sourceSquare / 8);
	if (pieceType == 6) return false;

	Bitboard movesBB;

	switch (pieceType) {
	case PAWN:		movesBB = generatePawnMoves(move.sourceSquare, color); break;
	case KNIGHT:	movesBB = generateKnightMoves(move.sourceSquare, color); break;
	case BISHOP:	movesBB = generateBishopMoves(move.sourceSquare, color); break;
	case ROOK:		movesBB = generateRookMoves(move.sourceSquare, color); break;
	case QUEEN:		movesBB = generateQueenMoves(move.sourceSquare, color); break;
	case KING:		movesBB = generateKingMoves(move.sourceSquare, color); break;
	default:		movesBB = 0; break;
	}

	move.moveType = determineMoveType(move);

	// Aplicar movimiento temporalmente
	MoveResult result = makeMove(move);
	if (result.success) {
		// Verificar si el rey está en jaque después del movimiento
		bool inCheck = scanChecks(color);
		undoMove();  // Revertir el movimiento
		if (inCheck) return false;

	}

	switch (move.moveType) {
	case PROMOTION:cout << "\npromotion"; break;
	case CASTLING:cout << "\ncastling"; break;
	case EN_PASSANT:cout << "\n en passant"; break;
	case CAPTURE:cout << "\ncaptura"; break;
	case DEFAULT:cout << "\nnormal move"; break;
	}
	return true;
}


MoveType Board::determineMoveType(const Move& move) const {
	const bool color = currentState.turn ? BLACK : WHITE;
	const Bitboard targetBB = 1ULL << move.targetSquare;

	// 1. Verificar promoción
	if (move.pieceType == PAWN) {
		const int promotionRank = (color == WHITE) ? 7 : 0;
		if (move.targetSquare / 8 == promotionRank) {
			
			return PROMOTION;
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
	move.moveType = determineMoveType(move);

	// 1. Obtener máscaras de bits
	Bitboard source = 1ULL << move.sourceSquare;
	Bitboard target = 1ULL << move.targetSquare;
	bool color = currentState.turn ? BLACK : WHITE;

	// 2. Verificar movimiento válido
	Bitboard validMoves;
	switch (move.pieceType) {
	case PAWN:   validMoves = generatePawnMoves(move.sourceSquare,color); break;
	case KNIGHT: validMoves = generateKnightMoves(move.sourceSquare, color); break;
	case BISHOP: validMoves = generateBishopMoves(move.sourceSquare, color);break;
	case ROOK:	 validMoves = generateRookMoves(move.sourceSquare, color);break;
	case QUEEN:	validMoves = generateQueenMoves(move.sourceSquare, color);break;
	case KING:	validMoves = generateKingMoves(move.sourceSquare,color) | generateCastlingMoves(color);break;
	default:	break;
	}
	if (!(validMoves & target)) return result;

	// 3. Guardar estado actual
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
		currentState.pieces[color][QUEEN] ^= target;    // Añadir reina
	}
	// Captura normal
	for (int pt = PAWN; pt <= KING; pt++) {
		if (currentState.pieces[!color][pt] & target) {
			result.capturedPiece = pt;
			result.capturedColor = !color;
			currentState.pieces[!color][pt] ^= target;
			break;
		}
	}
	

	// 6. Actualizar estado del juego
	currentState.enPassant = (move.pieceType == PAWN && abs(move.targetSquare - move.sourceSquare) == 16)
		? color? (1ULL << (move.sourceSquare - 8)): (1ULL << (move.sourceSquare + 8)) : 0;

	currentState.turn = !currentState.turn;
	result.success = true;
	return result;
}
void Board::undoMove() {
	if (moveCount == 0) return;

	moveCount--;
	currentState = prevStates[moveCount];
}




void Board::initAttackTables() {

	initBishopMagics();
	initRookMagics();


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
	/*
	// Precalcular máscaras de alfil (direcciones diagonales)
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

Bitboard Board::generatePawnMoves(int sq, bool color) const {
	Bitboard moves = 0;
	const Bitboard pawn = 1ULL << sq;
	const Bitboard empty = ~currentState.occupancy;
	const Bitboard enemies = getPieces(!color) | currentState.enPassant;
	const int forward = (color == WHITE) ? 8 : -8;

	// Movimiento simple (1 casilla)
	Bitboard singlePush = shift(pawn, forward) & empty;
	moves |= singlePush; // <- Añadir esta línea para incluir el movimiento simple

	// Movimiento doble (solo si está en fila inicial)
	Bitboard startRank = (color == WHITE) ? RANK_2 : RANK_7;
	if ((pawn & startRank) && singlePush) { // Verificar que singlePush sea válido
		Bitboard doublePush = shift(singlePush, forward) & empty;
		moves |= doublePush;
	}

	// Capturas diagonales (corregir direcciones y máscaras)
	Bitboard leftCapture = 0, rightCapture = 0;
	if (color == WHITE) {
		leftCapture = (pawn << 9) & ~FILE_A & enemies;  // Diagonal izquierda (no FILE_A)
		rightCapture = (pawn << 7) & ~FILE_H & enemies;  // Diagonal derecha (no FILE_H)
	}
	else {
		
		leftCapture = (pawn >> 7) & ~FILE_A & enemies;  // Diagonal derecha (no FILE_A)
		rightCapture = (pawn >> 9) & ~FILE_H & enemies;  // Diagonal izquierda (no FILE_H)
	}
	moves |= leftCapture | rightCapture;

	// En passant
	/*if (currentState.enPassant & (leftCapture | rightCapture)) {
		printBoard(leftCapture);
		printBoard(rightCapture);
		moves |= currentState.enPassant;
	}*/

	return moves;
}
Bitboard Board::generateRookMoves(int sq, bool color) const {
	Bitboard occupancy = currentState.occupancy;
	Bitboard rook = 1ULL << sq;

	// 1. Obtener ataques precalculados
	//magic
	/*Bitboard maskedOccupancy = occupancy & rookMasks[sq];
	int index = (maskedOccupancy * rookMagics[sq]) >> (64 - rookIndexBits[sq]);
	Bitboard attacks = rookAttacks[sq][index];*/

	//precalculado
	Bitboard maskedOccupancy = occupancy & rookMasks[sq];
	Bitboard attacks = getRookAttacksSlow(sq, maskedOccupancy);


	// 2. Filtrar piezas aliadas y bloquear detrás de enemigas
	Bitboard friendly = getPieces(color);
	//Bitboard enemies = getPieces(!color);

	
	/*Bitboard blockers = enemies & attacks;
	while (blockers) {
		int blocker_sq = portable_ctzll(blockers);
		Bitboard blocker_mask = ~((1ULL << blocker_sq) - 1); // Máscara para todo lo que está después del bloqueador
		attacks &= ~(rookMasks[sq] & blocker_mask);
		blockers &= blockers - 1;
	}*/
	return attacks & ~friendly; // Excluir piezas propias
}
Bitboard Board::generateBishopMoves(int sq, bool color) const {
	Bitboard occupancy = currentState.occupancy;
	Bitboard bishop = 1ULL << sq;

	// 1. Máscara de ataque base (excluyendo bordes)

	/*	int index = (maskedOccupancy * bishopMagics[sq]) >> (64 - bishopIndexBits[sq]);
	Bitboard attacks = bishopAttacks[sq][index];*/ 

	Bitboard maskedOccupancy = occupancy & bishopMasks[sq];
	Bitboard attacks = getBishopAttacksSlow(sq, maskedOccupancy);



	// 4. Filtrar piezas propias
	Bitboard friendly = getPieces(color);
	return attacks & ~friendly;
}
Bitboard Board::generateKnightMoves(int sq, bool color) const {
    Bitboard moves = knightMoves[sq];
    Bitboard friendly = getPieces(color);
    return moves & ~friendly; // Solo excluir casillas con piezas propias
}
Bitboard Board::generateQueenMoves(int sq, bool color) const {
	return generateRookMoves(sq, color) | generateBishopMoves(sq, color);
}
Bitboard Board::generateKingMoves(int sq, bool color) const {
	Bitboard moves = kingMoves[sq]; // Movimientos precalculados para "sq"
	Bitboard friendly = getPieces(color);
	moves &= ~friendly; // Filtrar casillas ocupadas por propias

	// Eliminar casillas bajo ataque enemigo
	Bitboard enemyAttacks = calculateAttackMap(!color);
	return moves & ~enemyAttacks;
}
Bitboard Board::generateCastlingMoves(bool color) const {
	Bitboard castling = 0;
	const Bitboard occupancy = currentState.occupancy;
	const Bitboard attacks = calculateAttackMap(!color);

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



Bitboard Board::getRookMask(int sq) const {
	Bitboard mask = 0;
	int x = sq % 8, y = sq / 8;

	// Horizontal (excluyendo bordes y casilla actual)
	for (int dx = -1; dx <= 1; dx += 2) {
		for (int cx = x + dx; cx >= 0 && cx < 8; cx += dx) {
			if (cx == 0 || cx == 7) break; // ¡Detener antes de incluir el borde!
			mask |= 1ULL << (y * 8 + cx);
		}
	}

	// Vertical (excluyendo bordes y casilla actual)
	for (int dy = -1; dy <= 1; dy += 2) {
		for (int cy = y + dy; cy >= 0 && cy < 8; cy += dy) {
			if (cy == 0 || cy == 7) break; // ¡Detener antes de incluir el borde!
			mask |= 1ULL << (cy * 8 + x);
		}
	}

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

Bitboard Board::getBishopMask(int sq) const {
	Bitboard mask = 0;
	int x = sq % 8, y = sq / 8;
	const int dirs[4][2] = { {1,1}, {-1,1}, {1,-1}, {-1,-1} };

	for (const auto& dir : dirs) {
		int dx = dir[0], dy = dir[1];
		int cx = x + dx, cy = y + dy;

		// Excluir bordes (detenerse antes de 0 y 7)
		while (cx >= 1 && cx <= 6 && cy >= 1 && cy <= 6) {
			mask |= 1ULL << (cy * 8 + cx);
			cx += dx;
			cy += dy;
		}
	}
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



bool Board::scanChecks(bool color, Bitboard& attackers) {
	attackers = 0;
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return true;


	// Calcular ataques enemigos al cuadrado del rey
	Bitboard attackersBB = calculateAttackMap(!color) & kingBB;

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
bool Board::scanChecks(bool color) {
	Bitboard null = 0;
	return scanChecks(color, null);
}

bool Board::canKingMove(bool color) {
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return false; // No hay rey, algo está mal

	// Obtener posición del rey
	int kingSquare = portable_ctzll(kingBB);

	// Generar todos los movimientos posibles del rey
	Bitboard kingmoves = kingMoves[kingSquare];

	// Filtrar movimientos que no están ocupados por piezas propias
	kingmoves &= ~getPieces(color);

	// Probar cada movimiento potencial
	while (kingmoves) {
		int targetSquare = portable_ctzll(kingmoves);
		kingmoves &= kingmoves - 1;

		// Crear movimiento temporal
		Move move;
		move.sourceSquare = kingSquare;
		move.targetSquare = targetSquare;
		move.pieceType = KING;
		move.moveType = DEFAULT; // El tipo se determinará automáticamente

		// Hacer el movimiento temporalmente
		MoveResult result = makeMove(move);
		if (!result.success) continue;

		// Verificar si el rey sigue en jaque después del movimiento
		Bitboard newKingBB = currentState.pieces[color][KING];
		int newKingSquare = portable_ctzll(newKingBB);
		bool inCheck = (calculateAttackMap(!color) & newKingBB) != 0;

		// Deshacer el movimiento
		undoMove();

		// Si encontramos al menos un movimiento que no deja al rey en jaque
		if (!inCheck) {
			return true;
		}
	}

	return false;
}
bool Board::canCaptureAttacker(bool color, Bitboard attackerBB) {
	// Get the attacker's square
	int attackerSquare = portable_ctzll(attackerBB);

	// Generate all moves for the current color
	vector<Move> moves = generateAllMoves(color);

	// Check if any move can capture the attacker
	for (const Move& move : moves) {
		if (move.targetSquare == attackerSquare) {
			// Temporarily make the move
			MoveResult result = makeMove(const_cast<Move&>(move));
			if (!result.success) continue;

			// Check if king is still in check after the move
			Bitboard kingBB = currentState.pieces[color][KING];
			bool stillInCheck = (calculateAttackMap(!color) & kingBB);

			// Undo the move
			undoMove();

			if (!stillInCheck) {
				return true;
			}
		}
	}
	return false;
}
bool Board::canBlockAttacker(bool color, Bitboard attackerBB) {
	Bitboard kingBB = currentState.pieces[color][KING];
	if (!kingBB) return false;

	int kingSquare = portable_ctzll(kingBB);
	int attackerSquare = portable_ctzll(attackerBB);

	// Get the path between king and attacker (for sliding pieces)
	int attackerType = BitboardGetType(attackerSquare % 8, attackerSquare / 8);

	// Only sliding pieces (queen, rook, bishop) can be blocked
	if (attackerType != QUEEN && attackerType != ROOK && attackerType != BISHOP) {
		return false;
	}

	// Calculate the path squares between king and attacker
	vector<int> pathSquares;
	int dx = (attackerSquare % 8) - (kingSquare % 8);
	int dy = (attackerSquare / 8) - (kingSquare / 8);

	dx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
	dy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

	int x = kingSquare % 8 + dx;
	int y = kingSquare / 8 + dy;

	while (x != attackerSquare % 8 || y != attackerSquare / 8) {
		pathSquares.push_back(y * 8 + x);
		x += dx;
		y += dy;
	}

	// Check if any piece can move to a square in the path
	vector<Move> moves = generateAllMoves(color);
	for (const Move& move : moves) {
		for (int square : pathSquares) {
			if (move.targetSquare == square) {
				// Temporarily make the move
				MoveResult result = makeMove(const_cast<Move&>(move));
				if (!result.success) continue;

				// Check if king is still in check after the move
				kingBB = currentState.pieces[color][KING];
				bool stillInCheck = (calculateAttackMap(!color) & kingBB);

				// Undo the move
				undoMove();

				if (!stillInCheck) {
					return true;
				}
			}
		}
	}

	return false;
}
bool Board::scanCheckMate(bool color) {
	Bitboard attackers;
	bool inCheck = scanChecks(color, attackers);

	if (!inCheck) return false; // Not in check, so not checkmate

	// If there are multiple attackers, only king moves can help
	if (countBits(attackers) > 1) {
		return !canKingMove(color);
	}

	// Check if king can move out of check
	if (canKingMove(color)) {
		return false;
	}

	// Check if attacker can be captured
	if (canCaptureAttacker(color, attackers)) {
		return false;
	}

	// Check if attack can be blocked (only for sliding pieces)
	int attackerSquare = portable_ctzll(attackers);
	int attackerType = BitboardGetType(attackerSquare % 8, attackerSquare / 8);

	if (attackerType == QUEEN || attackerType == ROOK || attackerType == BISHOP) {
		if (canBlockAttacker(color, attackers)) {
			return false;
		}
	}

	// If none of the above, it's checkmate
	return true;
}
