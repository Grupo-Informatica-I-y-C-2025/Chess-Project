#pragma once
#include "Bot_V4.h"
#include <random>
#include <chrono>

#define OPENING 0
#define MIDGAME 1
#define ENDGAME 2

const int INIT_MIN = -100000000; // -10^9
const int INIT_MAX = 100000000;  // 10^9

const int QUIESCE_DEPTH = 3;

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


void Bot_V4::initializeZobrist() {
	std::mt19937_64 rng(12345);
	for (int i = 0; i < 64; ++i)
		for (int j = 0; j < 12; ++j)
			zobristTable[i][j] = rng();
	zobristTurn = rng();
	for (int i = 0; i < 4; ++i) zobristCastling[i] = rng();
	for (int i = 0; i < 8; ++i) zobristEnPassant[i] = rng();
}

// Cálculo completo inicial del hash (se llama al inicio del juego)
uint64_t Bot_V4::computeFullHash() {
	uint64_t hash = 0;

	for (int sq = 0; sq < 64; ++sq) {
		int piece = board->BitboardGetType(sq);
		if (piece == NONE) continue;
		int color = board->BitboardGetColor(sq);
		int idx = color * 6 + piece;
		hash ^= zobristTable[sq][idx];
	}

	// En passant
	if (board->currentState.enPassant != 0) {
		int file = board->portable_ctzll(board->currentState.enPassant);
		hash ^= zobristEnPassant[file];
	}

	// Castling rights
	for (int i = 0; i < 4; ++i) {
		if (board->currentState.castlingRights & (1 << i))
			hash ^= zobristCastling[i];
	}

	// Turno
	if (board->currentState.turn == BLACK)
		hash ^= zobristTurn;

	return hash;
}

void Bot_V4::updateHash(const Move& move, const MoveResult& result) {
	int pieceIdx = board->BitboardGetColor(move.sourceSquare) * 6 + move.pieceType;

	// XOR pieza desde origen y destino
	currentHash ^= zobristTable[move.sourceSquare][pieceIdx];
	currentHash ^= zobristTable[move.targetSquare][pieceIdx];

	// Captura
	if (result.capturedPiece != NONE) {
		int capIdx = result.capturedColor * 6 + result.capturedPiece;
		currentHash ^= zobristTable[move.targetSquare][capIdx];
	}

	// En passant: eliminar valor antiguo si había
	if (result.oldEnPassant) {
		int file = board->portable_ctzll(result.oldEnPassant)%8;
		currentHash ^= zobristEnPassant[file];
	}

	// Nuevo en passant
	if (board->currentState.enPassant) {
		int file = board->portable_ctzll(board->currentState.enPassant)%8;
		currentHash ^= zobristEnPassant[file];
	}

	// Derechos de enroque previos
	for (int i = 0; i < 4; ++i) {
		bool prev = (result.oldCastlingRights >> i) & 1;
		bool now = (board->currentState.castlingRights >> i) & 1;
		if (prev != now) {
			currentHash ^= zobristCastling[i];
		}
	}

	// Turno
	currentHash ^= zobristTurn;
}

void Bot_V4::revertHash(const Move& move, const MoveResult& result) {
	updateHash(move, result);
}




int Bot_V4::piecePunctuation(int type) {
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

// Siempre devuelve (scoreBlancas − scoreNegras)
int Bot_V4::EvaluateGame() {
	int w = evaluateColor(WHITE);
	int b = evaluateColor(BLACK);
	return w - b;
}

// Evalúa todos los términos para un color concreto
int Bot_V4::evaluateColor(bool color) {
	int score = 0;
	if (board->scanCheckMate(color) || board->scanRepetition()) {
		return INIT_MIN;
	}
	
	int material = materialEvaluation(color) * 5;
	int mobility = mobilityEvaluation(color) * 5;
	int activity = pieceActivityEvaluation(color) * 10;
	int structure= pawnStructureEvaluation(color) * 15;
	int control = centerControlEvaluation(color) * 30;
	int safety = -kingSafetyEvaluation(color) * 10;
	score += bishopPairEvaluation(color) * 50;
	score += rookOpenFileEvaluation(color) * 20;
	score -= badBishopEvaluation(color) * 15;
	score += seventhRankEvaluation(color) * 20;
	int position = positionalEvaluation(color) / 2;
	/*if (color == WHITE) {
		cout << "\ncolor: white ";
		cout << "\nmaterial: " << material;
		cout << "\nmobility: " << mobility;
		cout << "\nactivity: " << activity;
		cout << "\nstructure: " << structure;
		cout << "\ncontrol: " << control;
		cout << "\nsafety: " << safety;
		cout << "\nposition: " << position;
	}*/
	score += material + mobility + activity + structure + control + safety + position;
	return score;
}

int Bot_V4::materialEvaluation(bool turn) {
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
				if ((1ULL << sq) & defense)score += 0.3 * points;
				if ((1ULL << sq) & threats)score -= 0.5 * points;
			}
		}
	}
	return score;
}

int Bot_V4::pieceActivityEvaluation(bool color) {
	int activity = 0;
	// Índices 0=PAWN,1=KNIGHT,2=BISHOP,3=ROOK,4=QUEEN,5=KING
	static const int WEIGHTS[6] = { 1, 4, 4, 2, 1, 0 };

	// Mapa de piezas propias
	Bitboard ownPieces = board->getPieces(color);
	// Mapa de ataques enemigos
	Bitboard enemyAttackMap = board->getAttackMap(!color);

	for (int pt = PAWN; pt <= QUEEN; ++pt) {
		Bitboard pieces = board->currentState.pieces[color][pt];
		while (pieces) {
			int sq = board->portable_ctzll(pieces);
			pieces &= pieces - 1;

			// Movimientos legales a casillas sin piezas propias
			Bitboard moves = board->generateMovesFrom(sq, color) & ~ownPieces;
			int totalMoves = board->countBits(moves);

			// Movimientos "seguros": que no van a casillas bajo ataque enemigo
			int safeMoves = board->countBits(moves & ~enemyAttackMap);

			// Actividad = safeMoves + (totalMoves - safeMoves)/2
			activity += WEIGHTS[pt] * (safeMoves + (totalMoves - safeMoves) / 2);
		}
	}
	return activity;
}

int Bot_V4::mobilityEvaluation(bool color) {
	int mobility = 0;
	for (int pt = KNIGHT; pt <= QUEEN; ++pt) {
		Bitboard pieces = board->currentState.pieces[color][pt];
		while (pieces) {
			int sq = board->portable_ctzll(pieces);
			pieces &= pieces - 1;
			Bitboard moves = board->generateMovesFrom(sq, color) & ~board->getPieces(color);
			mobility += board->countBits(moves);
		}
	}
	return mobility;
}

int Bot_V4::pawnStructureEvaluation(bool color) {
	Bitboard pawns = board->currentState.pieces[color][PAWN];
	Bitboard enemyPawns = board->currentState.pieces[!color][PAWN];
	int score = 0;

	// Dobles: penalización 50 por peón adicional en la misma columna
	for (int file = 0; file < 8; ++file) {
		Bitboard fileMask = Board::FILE_MASKS[file] & pawns;
		int count = board->countBits(fileMask);
		if (count > 1) score -= 50 * (count - 1);
	}

	// Aislados: penalización 25 por cada peón aislado
	for (int file = 0; file < 8; ++file) {
		Bitboard fileMask = Board::FILE_MASKS[file];
		Bitboard thisFile = pawns & fileMask;
		if (thisFile) {
			Bitboard adjMask = 0ULL;
			if (file > 0) adjMask |= Board::FILE_MASKS[file - 1];
			if (file < 7) adjMask |= Board::FILE_MASKS[file + 1];
			if (!(pawns & adjMask)) {
				score -= 25 * board->countBits(thisFile);
			}
		}
	}

	// Pasados: bonificación 20 + 10*avance de rango
	Bitboard passed = pawns;
	while (passed) {
		int sq = board->portable_ctzll(passed);
		passed &= passed - 1;
		int rank = (color == WHITE) ? (sq / 8) : (7 - (sq / 8));
		Bitboard blockMask = board->PASSED_PAWN_MASKS[color][sq];
		if (!(enemyPawns & blockMask)) {
			score += 20 + 10 * rank;
		}
	}

	return score;
}

int Bot_V4::centerControlEvaluation(bool color) {
	// Casillas centrales: d4, e4, d5, e5
	Bitboard center = Board::CENTER_SQUARES;
	Bitboard ownControl = board->getAttackMap(color) & center;
	Bitboard enemyControl = board->getAttackMap(!color) & center;
	int ownCount = board->countBits(ownControl);
	int enemyCount = board->countBits(enemyControl);
	// Bonificación 3 centi-puntos por control propio, penalización 2 por control enemigo
	return ownCount * 3 - enemyCount * 2;
}

int Bot_V4::kingSafetyEvaluation(bool color) {
	// Encontrar casilla del rey
	Bitboard kingBB = board->currentState.pieces[color][KING];
	int kingSq = board->portable_ctzll(kingBB);

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
	Bitboard enemyAttacks = board->calculateAttackMap(!color) & zone;
	int attackCount = board->countBits(enemyAttacks);
	int attackWeight = attackCount * 25; // 25 centi-puntos por atacante

	// Bonus por peones del escudo del rey en zona
	Bitboard pawnShield = board->currentState.pieces[color][PAWN] & zone;
	int shieldCount = board->countBits(pawnShield);
	int shieldBonus = shieldCount * 30;

	return attackWeight - shieldBonus;
}

int Bot_V4::bishopPairEvaluation(bool color) {
	int count = board->countBits(board->currentState.pieces[color][BISHOP]);
	return (count >= 2) ? 100 : 0;
}

int Bot_V4::rookOpenFileEvaluation(bool color) {
	int bonus = 0;
	Bitboard rooks = board->currentState.pieces[color][ROOK];
	Bitboard ownPawns = board->currentState.pieces[color][PAWN];
	Bitboard enemyPawns = board->currentState.pieces[!color][PAWN];
	while (rooks) {
		int sq = board->portable_ctzll(rooks);
		rooks &= rooks - 1;
		int file = sq % 8;
		Bitboard fileMask = Board::FILE_MASKS[file];
		bool open = !(ownPawns & fileMask);
		bool semiOpen = !(enemyPawns & fileMask);
		if (open)      bonus += 50;
		else if (semiOpen) bonus += 20;
	}
	return bonus;
}

int Bot_V4::badBishopEvaluation(bool color) {
	int penalty = 0;
	Bitboard bishops = board->currentState.pieces[color][BISHOP];
	Bitboard ownPieces = board->getPieces(color);
	while (bishops) {
		int sq = board->portable_ctzll(bishops);
		bishops &= bishops - 1;
		Bitboard moves = board->generateMovesFrom(sq, color) & ~ownPieces;
		if (board->countBits(moves) < 4) penalty += 30;
	}
	return penalty;
}

int Bot_V4::seventhRankEvaluation(bool color) {
	int bonus = 0;
	int rank = (color == WHITE) ? 6 : 1;  // séptima fila para cada color
	Bitboard mask = Board::RANK_MASKS[rank];
	Bitboard heavy = board->currentState.pieces[color][ROOK] |
		board->currentState.pieces[color][QUEEN];
	bonus = board->countBits(heavy & mask) * 25;
	return bonus;
}


int Bot_V4::gamePhase() {
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

int Bot_V4::positionalEvaluation(bool color) {
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



vector<Move> Bot_V4::generateAllMoves(bool color) {
	auto moves = board->generateAllMoves(color);
	orderMoves(moves);
	return moves;
}

vector<Move> Bot_V4::generateQuiescentMoves(bool color) {
	auto moves = board->generateQuiescientMoves(color);
	orderMoves(moves);
	return moves;
}

vector<Move> Bot_V4::generateCheckMoves(bool color) {
	auto moves = board->generateCheckMoves(color);
	orderMoves(moves);
	return moves;
}

void Bot_V4::orderMoves(vector<Move>& moves) {
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
		Move& m = moves[i];

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
			/*if (isCapture) {
				int seeVal = staticExchangeEval(m);
				score += 1000 * seeVal;
			}*/
			if (m.moveType == PROMOTION) {
				score += 1000 * piecePunctuation(m.promotionPiece);
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


static int totalNodes = 0;
static int totalTTHits = 0;

Bot_V4::search_result Bot_V4::SearchMoves(int depth, int alpha, int beta) {
	totalNodes++;
	if (std::count(history.begin(), history.end(), currentHash) >= 3)
		return { 0, Move{0,0,NONE,DEFAULT} };

	int alphaOrig = alpha, betaOrig = beta;
	auto it = transpositionTable.find(currentHash);
	if (it != transpositionTable.end() && it->second.depth >= depth) {
		totalTTHits++;
		auto& e = it->second;
		if (e.flag == TTEntry::EXACT)
			return { e.evaluation, Move{0,0,NONE,DEFAULT} };
		if (e.flag == TTEntry::LOWERBOUND)
			alpha = max(alpha, e.evaluation);
		else if (e.flag == TTEntry::UPPERBOUND)
			beta = min(beta, e.evaluation);
		if (alpha >= beta)
			return { e.evaluation, Move{0,0,NONE,DEFAULT} };
	}

	if (depth == 0)
		return SearchQuiescent(alpha, beta);

	bool maximizing = (board->currentState.turn == WHITE);

	int bestEval = maximizing ? INIT_MIN : INIT_MAX;
	Move bestMove = { 0,0,NONE,DEFAULT };

	auto moves = generateAllMoves(board->currentState.turn);
	orderMoves(moves);

	for (auto& m : moves) {
		auto res = board->makeMove(m);
		if (!res.success) continue;
		currentHash = computeFullHash();
		history.push_back(currentHash);

		search_result sr;
		if (m.ischeck)sr = SearchChecks(alpha, beta);
		else sr = SearchMoves(depth - 1, alpha, beta);

		history.pop_back();
		board->undoMove();

		if ((maximizing && sr.evaluation > bestEval) || (!maximizing && sr.evaluation < bestEval)) {
			bestEval = sr.evaluation;
			bestMove = m;
		}
		if (maximizing) alpha = max(alpha, bestEval);
		else beta = min(beta, bestEval);
		if (beta <= alpha) break;
	}

	TTEntry::BoundType flag = TTEntry::EXACT;
	if (bestEval <= alphaOrig) flag = TTEntry::UPPERBOUND;
	else if (bestEval >= betaOrig) flag = TTEntry::LOWERBOUND;
	transpositionTable[currentHash] = { depth, bestEval, flag };

	if (bestMove.sourceSquare == 0 && bestMove.targetSquare == 0) {
		if (maximizing) return { INIT_MIN + depth, Move{0,0,NONE,DEFAULT} }; // Blancas pierden
		else return { INIT_MAX - depth, Move{0,0,NONE,DEFAULT} };            // Negras pierden
	}
	return { bestEval, bestMove };
}

Bot_V4::search_result Bot_V4::SearchQuiescent(int alpha, int beta) {
	if (std::count(history.begin(), history.end(), currentHash) >= 3)
		return { 0, Move{0,0,NONE,DEFAULT} };

	bool maximizing = (board->currentState.turn == 0); // 0 = WHITE
	int standPat;
	auto itc = evalCache.find(currentHash);
	if (itc != evalCache.end()) {
		standPat = itc->second;
	}
	else {
		standPat = EvaluateGame();
		evalCache[currentHash] = standPat;
	}

	if (standPat >= beta) return { standPat, Move{0,0,NONE,DEFAULT} };
	if (standPat > alpha) alpha = standPat;

	auto moves = generateQuiescentMoves(board->currentState.turn);

	int bestEval = standPat;
	Move bestMove = Move{ 0,0,NONE,DEFAULT };

	for (auto& m : moves) {
		if (staticExchangeEval(m) < -20) continue;

		auto res = board->makeMove(m);
		if (!res.success) continue;
		currentHash = computeFullHash();
		history.push_back(currentHash);

		auto sr = SearchQuiescent(alpha, beta);

		history.pop_back();
		board->undoMove();

		if ((maximizing && sr.evaluation > bestEval) || (!maximizing && sr.evaluation < bestEval)) {
			bestEval = sr.evaluation;
			bestMove = m;
		}

		if (maximizing) alpha = std::max(alpha, bestEval);
		else beta = std::min(beta, bestEval);

		if (beta <= alpha) break;
	}

	return { bestEval, bestMove };
}

Bot_V4::search_result Bot_V4::SearchChecks(int alpha, int beta) {
	if (std::count(history.begin(), history.end(), currentHash) >= 3)
		return { 0, Move{0,0,NONE,DEFAULT} };

	bool maximizing = (board->currentState.turn == WHITE);
	int standPat;
	auto itc = evalCache.find(currentHash);
	if (itc != evalCache.end()) {
		standPat = itc->second;
	}
	else {
		standPat = EvaluateGame();
		evalCache[currentHash] = standPat;
	}
	int bestEval = standPat;
	Move bestMove = Move{ 0,0,NONE,DEFAULT };

	// Movimientos que dan jaque
	auto moves = generateCheckMoves(board->currentState.turn);
	if (moves.empty()) {
		// Si ya no quedan jaques, volvemos a la búsqueda quiesciente
		return SearchQuiescent( alpha, beta);
	}

	for (auto& m : moves) {
		auto res = board->makeMove(m);
		if (!res.success) continue;
		currentHash = computeFullHash();
		history.push_back(currentHash);

		// No reducimos profundidad: seguimos explorando jaques
		auto sr = SearchChecks(alpha, beta);

		history.pop_back();
		board->undoMove();

		if ((maximizing && sr.evaluation > bestEval) ||
			(!maximizing && sr.evaluation < bestEval)) {
			bestEval = sr.evaluation;
			bestMove = m;
		}

		if (maximizing) alpha = std::max(alpha, bestEval);
		else            beta = std::min(beta, bestEval);
		if (beta <= alpha) break;
	}

	return { bestEval, bestMove };
}

int Bot_V4::staticExchangeEval(const Move& move) {
	int target = move.targetSquare;
	int gain[32];
	int depth = 0;

	gain[depth] = piecePunctuation(board->BitboardGetType(target));

	Bitboard attackersWhite = board->attackersTo(target, WHITE);
	Bitboard attackersBlack = board->attackersTo(target, BLACK);

	bool side = board->currentState.turn;

	Bitboard fromBB = board->squareToBitboard(move.sourceSquare);
	if (side == WHITE)
		attackersWhite &= ~fromBB;
	else
		attackersBlack &= ~fromBB;

	while (true) {
		side = (side == WHITE) ? BLACK : WHITE;
		Bitboard attackers = (side == WHITE) ? attackersWhite : attackersBlack;

		if (attackers == 0)
			break;

		int from = selectLeastValuableAttacker(attackers, side);
		int pt = board->BitboardGetType(from);

		depth++;
		gain[depth] = piecePunctuation(pt) - gain[depth - 1];

		if (gain[depth] < 0)
			break;

		Bitboard fromBB = board->squareToBitboard(from);
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

int Bot_V4::selectLeastValuableAttacker(Bitboard attackers, bool color) {
	int minValue = 999999; // Valor inicial alto
	int selectedSquare = -1;

	while (attackers) {
		int sq = board->portable_ctzll(attackers);
		attackers &= attackers - 1; // Eliminar el bit procesado

		// Obtener tipo de pieza y su valor
		int type = board->BitboardGetType(sq);
		int value = piecePunctuation(type);

		// Actualizar mínimo
		if (value < minValue || (value == minValue && sq < selectedSquare)) {
			minValue = value;
			selectedSquare = sq;
		}
	}

	return selectedSquare;
}



Move Bot_V4::botMove(bool turn) {
	// Inicializar métricas y tabla de transposición
	totalNodes = 0;
	totalTTHits = 0;
	clearTranspositionTable();
	transpositionTable.clear();
	transpositionTable.reserve(1 << 20);  // ~1M entradas
	history.clear();
	// Búsqueda iterativa para profundizar
	Move bestMove;
	search_result sr;
	int maxDepth = 5; // ajustar según rendimiento
	for (int d = 1; d <= maxDepth; ++d) {
		cout << "\n--- Search Depth " << d << " ---" << endl;
		sr = SearchMoves(d, INIT_MIN, INIT_MAX);
		cout << "BOT4 Evaluación: " << sr.evaluation << endl;
		cout << "BOT4 Nodos: " << totalNodes << endl;
		cout << "BOT4 TTHits: " << totalTTHits << endl;
		double hitRate = (double) 100.0 * totalTTHits / totalNodes;
		cout << "BOT4 Hits%: " << hitRate << "%" << endl;
		bestMove = sr.bestMove;
	}
	return bestMove;
}
