#include "board.h"
#include "generator.h"





//Bundle move generation
void Generator::updateAttackMap( Board& board) {
    board.currentAttacks[0] = calculateAttackMap(0,board);
    board.currentAttacks[1] = calculateAttackMap(1,board);
}

/*Interfaz */Bitboard Generator::generateAttacksFrom(int sq, bool color,const Board& board) {
	int pieceType = board.currentState.pieceType[sq];
	switch (pieceType) {
	case PAWN: return board.pawnAttacks[color][sq] ;
	case KNIGHT: return board.knightMoves[sq];
	case BISHOP: return generateBishopMoves(sq, color,board);
	case ROOK: return generateRookMoves(sq, color,board);
	case QUEEN: return generateQueenMoves(sq, color,board);
	case KING: return board.kingMoves[sq];
	default: return 0;
	}
}
Bitboard Generator::calculateAttackMap(bool color,const Board& board)  {
	Bitboard attacks = 0;
	Bitboard pieces = board.getPieces(color);

	while (pieces) {
		int sq = portable_ctzll(pieces);
		attacks |= generateAttacksFrom(sq, color,board);
		pieces &= pieces - 1;
	}
	return attacks;
}

Bitboard Generator::generateMovesFrom(int sq, bool color,const Board& board)  {
	int pieceType = board.currentState.pieceType[sq];
	switch (pieceType) {
	case PAWN: return generatePawnMoves(sq, color,board);
	case KNIGHT: return generateKnightMoves(sq, color,board);
	case BISHOP: return generateBishopMoves(sq, color,board);
	case ROOK: return generateRookMoves(sq, color,board);
	case QUEEN: return generateQueenMoves(sq, color,board);
	case KING: return generateKingMoves(sq, color,board);
	default: return 0;
	}
}
Bitboard Generator::getMovementMap(bool color,const Board& board) {
	Bitboard movementMap = 0;
	for (int pt = PAWN; pt <= KING; pt++) {
		Bitboard pieces = board.currentState.pieces[color][pt];
		while (pieces) {
			int sq = portable_ctzll(pieces);
			movementMap |= generateMovesFrom(sq, color,board);
			pieces &= pieces - 1;
		}
	}
	return movementMap & ~board.getPieces(color); // Excluye casillas ocupadas por propias
}

vector<Move> Generator::generateAllMoves(bool color, Board& board) {
	vector<Move> allMoves;
	Bitboard kingBB = board.currentState.pieces[color][KING];
	if (!kingBB) return allMoves;

	for (int source = 0; source < 64; ++source) {
		if (board.currentState.pieceColor[source] != color)continue;
		int pieceType = board.currentState.pieceType[source];

		Bitboard movesBB = generateMovesFrom(source,color,board);

		while (movesBB) {
			int target = portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			if (isLegalmove(move,board)) {
				if(move.moveType==PROMOTION){
					if (board.getVariant()==0) {
						move.promotionPiece = QUEEN;
						allMoves.push_back(move);
						move.promotionPiece = KNIGHT;
						allMoves.push_back(move);
						move.promotionPiece = BISHOP;
						allMoves.push_back(move);
						move.promotionPiece = ROOK;
						allMoves.push_back(move);
					}
					if (board.getVariant() == 1) {
						move.promotionPiece = KNIGHT;
						allMoves.push_back(move);
						move.promotionPiece = BISHOP;
						allMoves.push_back(move);
						move.promotionPiece = ROOK;
						allMoves.push_back(move);
					}
					if (board.getVariant() == 2) {
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
vector<Move> Generator::generateCaptureMoves(bool color, Board& board) {
	vector<Move> allMoves;
	Bitboard kingBB = board.currentState.pieces[color][KING];
	if (!kingBB) return allMoves;
	Bitboard enemyPieces = board.getPieces(!color) | board.currentState.enPassant;

	for (int source = 0; source < 64; ++source) {
		if (board.currentState.pieceColor[source] != color)continue;
		int pieceType = board.currentState.pieceType[source];

		Bitboard movesBB = generateAttacksFrom(source, color,board);
		movesBB &= enemyPieces;

		while (movesBB) {
			int target = portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			if (isLegalmove(move,board))allMoves.push_back(move);
		}
	}
	return allMoves;
}
vector<Move> Generator::generateQuiescientMoves(bool color, Board& board) {
	vector<Move> allMoves;
	for  (auto& move : generateAllMoves(color,board)) {
		if (move.ischeck)allMoves.push_back(move);
		if (move.moveType == CAPTURE || move.moveType == EN_PASSANT || move.moveType == PROMOTION)allMoves.push_back(move);
		
	}
	return allMoves;
}
vector<Move> Generator::generateCheckMoves(bool color, Board& board) {
	vector<Move> allMoves;
	for (auto& move : generateAllMoves(color,board)) {
		if (move.ischeck)allMoves.push_back(move);
	}
	return allMoves;
}

void Generator::orderMoves(vector<Move>& moves, Board& board) {
	if (moves.empty()) return;

	// Obtener el color del jugador que realiza los movimientos
	bool color = board.BitboardGetColor(moves[0].sourceSquare);

	// Precalcular puntuaciones
	vector<pair<int, int>> scoredMoves; // (score, index)
	for (size_t i = 0; i < moves.size(); ++i) {
		int score = 0;
		bool isCheck = false;
		bool isCapture = false;
		int victimValue = 0;
		Move& m = moves[i];

		// 1. Verificar si el movimiento da jaque
		makeMove(moves[i],board);
		isCheck = board.scanChecks(!color); // Verificar jaque al oponente
		undoMove(board);

		if (isCheck) {
			score = 50000;
			if (board.scanCheckMate(!color)) score += 1000000;
		}
		else {
			// 2. Verificar si es captura (normal o al paso)
			Bitboard targetBB = 1ULL << moves[i].targetSquare;
			isCapture = (board.getPieces(!color) & targetBB) ||
				(moves[i].pieceType == PAWN && (board.currentState.enPassant & targetBB));

			if (isCapture) {
				// Calcular MVV-LVA
				if (board.getPieces(!color) & targetBB) {
					victimValue = board.piecePunctuation(board.BitboardGetType(moves[i].targetSquare));
				}
				else { // Captura al paso
					victimValue = 100; // Valor de peón
				}
				int aggressorValue = board.piecePunctuation(moves[i].pieceType);
				score = 10000 + (victimValue - aggressorValue); // MVV-LVA
			}
			/*if (isCapture) {
				int seeVal = staticExchangeEval(m);
				score += 1000 * seeVal;
			}*/
			if (m.moveType == PROMOTION) {
				score += 1000 * board.piecePunctuation(m.promotionPiece);
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



//move execution
bool Generator::isLegalmove(Move& move, Board& board) {
	move.pieceType = board.currentState.pieceType[move.sourceSquare];
	bool color = board.currentState.turn;
	int targetColor = board.currentState.pieceColor[move.targetSquare];
	Bitboard sourceBB = 1ULL << move.sourceSquare;
	Bitboard targetBB = 1ULL << move.targetSquare;
	//casilla vacia
	if (!(board.currentState.pieces[color][move.pieceType] & sourceBB)) {
		return false;
	}
	//kramnik
	if (targetColor == color) {
		if(!board.isKramnik())return false;
	}
	//destino fuera de tablero (no 8*8)
	if (targetBB & board.boundary) return false;
	Bitboard movesBB = generateMovesFrom(move.sourceSquare, color,board);
	if (!board.isKramnik())movesBB & ~board.getPieces(color);

	if (!(movesBB & (1ULL << move.targetSquare)))return false;

	MoveResult result = makeMove(move,board);
	if (!result.success) return false;
	move.ischeck = board.scanChecks(!color);
	bool inCheck = board.scanChecks(color);
	undoMove(board);
	
	return inCheck ? false : true;
}
MoveType Generator::determineMoveType(const Move& move, const Board& board)  {
	const bool color = board.currentState.turn ? BLACK : WHITE;
	const Bitboard targetBB = 1ULL << move.targetSquare;

	if (move.pieceType == PAWN) {
		if (board.getVariant()==2) {
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
	if (move.pieceType == PAWN && (targetBB & board.currentState.enPassant)) {
		
		return EN_PASSANT;
	}
	
	// 4. Verificar captura normal
	if (board.getPieces(!color) & targetBB) {
		
		return CAPTURE;
	}

	return DEFAULT;
}
MoveResult Generator::makeMove( Move& move, Board& board) {
	MoveResult result;
	result.success = false;

	// Guardar captura
	int sourceType = board.currentState.pieceType[move.sourceSquare];
	int sourceColor = board.currentState.pieceColor[move.sourceSquare];

	int targetType = board.currentState.pieceType[move.targetSquare];
	int targetColor = board.currentState.pieceColor[move.targetSquare];

	move.moveType = determineMoveType(move,board);
	if (move.moveType == PROMOTION) {
		bool valid = false;
		switch (board.getVariant()) {
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
	// 1. Obtener m�scaras de bits
	Bitboard source = 1ULL << move.sourceSquare;
	Bitboard target = 1ULL << move.targetSquare;
	bool color = board.currentState.turn ? BLACK : WHITE;

	// 3. Guardar estado actual
	if (board.moveCount >= 500) {
		return result;
	}
	board.prevStates[board.moveCount] = board.currentState;
	board.moveCount++;

	// 4. Realizar movimiento base
	board.currentState.pieces[color][move.pieceType] ^= source | target;
	board.currentState.occupancy ^= source | target;

	board.currentState.pieceType[move.sourceSquare] = EMPTY_CELL;
	board.currentState.pieceColor[move.sourceSquare] = NONE;

	board.currentState.pieceType[move.targetSquare] = sourceType;
	board.currentState.pieceColor[move.targetSquare] = sourceColor;

	if (move.moveType == EN_PASSANT) {
		int epSquare = move.targetSquare + (color == WHITE ? -8 : 8);
		result.capturedPiece = PAWN;
		result.capturedColor = !color;
		board.currentState.pieces[!color][PAWN] ^= (1ULL << epSquare);
		board.currentState.occupancy ^= (1ULL << epSquare);

		board.currentState.pieceType[epSquare] = EMPTY_CELL;
		board.currentState.pieceColor[epSquare] = NONE;
	}
	else if (move.moveType == CASTLING) {

		Bitboard castlingType;
		int delta = (move.targetSquare - move.sourceSquare);

		if ((board.currentState.castlingRights & WHITE_QUEENSIDE) && delta == -2 && move.sourceSquare == 4) {
			board.currentState.pieces[WHITE][ROOK] ^= 1ULL << 3;
			board.currentState.pieces[WHITE][ROOK] ^= 1ULL << 0;

			board.currentState.pieceType[3] = ROOK;
			board.currentState.pieceColor[3] = WHITE;

		}else if ((board.currentState.castlingRights & WHITE_KINGSIDE) && delta == 2 && move.sourceSquare == 4) {
			board.currentState.pieces[WHITE][ROOK] ^= 1ULL << 5;
			board.currentState.pieces[WHITE][ROOK] ^= 1ULL << 7;

			board.currentState.pieceType[5] = ROOK;
			board.currentState.pieceColor[5] = WHITE;

		}else if ((board.currentState.castlingRights & WHITE_QUEENSIDE) && delta == -2 && move.sourceSquare == 60) {
			board.currentState.pieces[BLACK][ROOK] ^= 1ULL << 59;
			board.currentState.pieces[BLACK][ROOK] ^= 1ULL << 56;

			board.currentState.pieceType[59] = ROOK;
			board.currentState.pieceColor[59] = BLACK;

		}else if ((board.currentState.castlingRights & WHITE_KINGSIDE) && delta == 2 && move.sourceSquare == 60) {
			board.currentState.pieces[BLACK][ROOK] ^= 1ULL << 61;
			board.currentState.pieces[BLACK][ROOK] ^= 1ULL << 63;

			board.currentState.pieceType[61] = ROOK;
			board.currentState.pieceColor[61] = BLACK;
		}
	}
	else if (move.pieceType == KING) {
		board.currentState.castlingRights &= ~(color ? (BLACK_KINGSIDE | BLACK_QUEENSIDE) : (WHITE_KINGSIDE | WHITE_QUEENSIDE));
	}
	else if (move.pieceType == ROOK) {
		// Check if the rook is on its initial square
		if (color == WHITE) {
			if (move.sourceSquare == 0) board.currentState.castlingRights &= ~WHITE_QUEENSIDE;
			else if (move.sourceSquare == 7) board.currentState.castlingRights &= ~WHITE_KINGSIDE;
		}
		else {
			if (move.sourceSquare == 56) board.currentState.castlingRights &= ~BLACK_QUEENSIDE;
			else if (move.sourceSquare == 63) board.currentState.castlingRights &= ~BLACK_KINGSIDE;
		}
	}
	else if (move.moveType == PROMOTION) {
		board.currentState.pieces[color][PAWN] ^= target;     // Eliminar pe�n
		board.currentState.pieces[color][move.promotionPiece] ^= target;    // A�adir reina

		board.currentState.pieceType[move.targetSquare] = move.promotionPiece;
		board.currentState.pieceColor[move.targetSquare] = sourceColor;
	}
	// Captura normal
	if (targetType != 6 && targetColor !=2) {
		if (board.isKramnik() || targetColor != color) {
			board.currentState.pieces[targetColor][targetType] ^= target;
			result.capturedPiece = targetType;
			result.capturedColor = targetColor;
		}
	}

	// 6. Actualizar estado del juego
	board.currentState.occupancy = 0;
	for (int i = PAWN; i <= KING; i++) {
		board.currentState.occupancy |= board.currentState.pieces[color][i] | board.currentState.pieces[!color][i];
	}
	updateAttackMap(board);
	board.currentState.enPassant = (move.pieceType == PAWN && abs(move.targetSquare - move.sourceSquare) == 16)
		? color? (1ULL << (move.sourceSquare - 8)): (1ULL << (move.sourceSquare + 8)) : 0;

	board.currentState.turn = !board.currentState.turn;
	result.success = true;
	return result;
}
void Generator::undoMove( Board& board) {
	if (board.moveCount == 0) return;
	//cout << "\n move count :" << moveCount;
	board.moveCount--;
	board.currentState = board.prevStates[board.moveCount];
}



//basic move generation
Bitboard Generator::generatePawnMoves(int sq, bool color,const Board& board)  {
	Bitboard moves = 0;
	const int x = sq % 8;
	const int y = sq / 8;

	const Bitboard pawn = 1ULL << sq;
	const Bitboard enemies = board.getPieces(!color) | board.currentState.enPassant;
	const Bitboard empty = ~board.currentState.occupancy;

	const int forward = (color == WHITE) ? 8 : -8;
	Bitboard singlePush = shift(pawn, forward) & empty;
	moves |= singlePush;

	if (board.getVariant()!=2) {
		if ((color == WHITE && y == 1) || (color == BLACK && y == 6)) {
			Bitboard doublePush = shift(singlePush, forward) & empty;
			moves |= doublePush;
		}
	}
	// Capturas usando m�scaras precalculadas
	moves |= board.pawnAttacks[color][sq] & enemies;

	return moves;
}
Bitboard Generator::generateRookMoves(int sq, bool color,const Board& board)  {
	Bitboard occ = board.currentState.occupancy;
	Bitboard mask = board.rookMasks[sq];
	Bitboard block = occ & mask;
	int      bits = board.rookIndexBits[sq];            // dynamic occupancy bits
	uint64_t magic = board.rookMagics[sq];
	uint64_t idx = (block * magic) >> (64 - bits);
	// guard against out-of-bounds
	if (idx >= (1u << bits)) return 0ULL;
	return board.rookAttacks[sq][idx];
}

Bitboard Generator::generateBishopMoves(int sq, bool color,const Board& board)  {
	Bitboard occ = board.currentState.occupancy;
	Bitboard mask = board.bishopMasks[sq];
	Bitboard block = occ & mask;
	int      bits = board.bishopIndexBits[sq];         // dynamic occupancy bits
	uint64_t magic = board.bishopMagics[sq];
	uint64_t idx = (block * magic) >> (64 - bits);
	if (idx >= (1u << bits)) return 0ULL;
	return board.bishopAttacks[sq][idx];
}
Bitboard Generator::generateKnightMoves(int sq, bool color,const Board& board)  {
    Bitboard moves = board.knightMoves[sq];
    return moves;
}
Bitboard Generator::generateQueenMoves(int sq, bool color,const Board& board)  {
	return generateRookMoves(sq, color,board) | generateBishopMoves(sq, color,board);
}
Bitboard Generator::generateKingMoves(int sq, bool color,const Board& board)  {
	Bitboard moves = board.kingMoves[sq]| generateCastlingMoves(color,board);
	Bitboard enemyAttacks = board.getAttackMap(!color);
	return moves & ~enemyAttacks;
}
Bitboard Generator::generateCastlingMoves(bool color,const Board& board)  {
	Bitboard castling = 0;
	if (!board.currentState.castlingRights)return castling;
	const Bitboard occupancy = board.currentState.occupancy;
	const Bitboard attacks = board.getAttackMap(!color);

	// Kingside castling
	if (board.currentState.castlingRights & (color ? BLACK_KINGSIDE : WHITE_KINGSIDE)) {
		const Bitboard path = color == WHITE ? 0x60ULL : 0x6000000000000000ULL;
		if (!(occupancy & path) && !(attacks & (path >> 1))) {
			castling |= color == WHITE ? 0x40ULL : 0x4000000000000000ULL;
		}
	}

	// Queenside castling
	if (board.currentState.castlingRights & (color ? BLACK_QUEENSIDE : WHITE_QUEENSIDE)) {
		const Bitboard path = color == WHITE ? 0xEULL : 0xE00000000000000ULL;
		if (!(occupancy & path) && !(attacks & (path << 1))) {
			castling |= color == WHITE ? 0x4ULL : 0x400000000000000ULL;
		}
	}

	return castling;
}

