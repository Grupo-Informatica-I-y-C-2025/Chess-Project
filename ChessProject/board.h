#ifndef __BOARD_H__
#define __BOARD_H__

#pragma once
#include <iostream>
#include <vector>

#include <bitset>
#include <climits>

#include <random>

#include <unordered_set>
#include <unordered_map>
#include <omp.h>

#include <fstream>
#include <algorithm>

#define WHITE_KINGSIDE  0x1
#define WHITE_QUEENSIDE 0x2
#define BLACK_KINGSIDE  0x4
#define BLACK_QUEENSIDE 0x8

struct GameType {
	bool kramnik;
	int x;
	int y;
};
const GameType games[6] = {
		{false, 8, 8},
		{true, 8, 8},
		{false, 4, 8} ,
		{ true, 4, 8 } ,
		{ false, 4, 5 } ,
		{ true, 4, 5 }
};

using namespace std;

enum MoveType {
	DEFAULT,
	CASTLING,
	EN_PASSANT,
	PROMOTION,
	CAPTURE
};
enum PieceType {
 PAWN = 0,
 KNIGHT = 1,
 BISHOP = 2,
 ROOK = 3,
 QUEEN = 4,
 KING = 5,
 EMPTY_CELL = 6,
};
enum PieceColor {
	WHITE = 0,
	BLACK = 1,
	NONE = 2,
};

typedef uint64_t Bitboard;

struct ChessState {
	// Bitboards por color y tipo [WHITE/BLACK][PIECE_TYPE]
	Bitboard pieces[2][6] = { {0} };
	Bitboard enPassant =  0 ;
	Bitboard castlingRights = 0;
	Bitboard occupancy = 0;
	Bitboard currentAttacks[2];
	Bitboard selected = 0;
	bool turn;

	bool operator==(ChessState const& o) const {
		if (turn != o.turn)            return false;
		if (occupancy != o.occupancy)       return false;
		if (enPassant != o.enPassant)       return false;
		if (castlingRights != o.castlingRights)  return false;

		// Compara el array de piezas
		for (int c = 0; c < 2; ++c)
			for (int pt = PAWN; pt <= KING; ++pt)
				if (pieces[c][pt] != o.pieces[c][pt])
					return false;

		return true;
	}

	bool operator!=(ChessState const& o) const {
		return !(*this == o);
	}

};
struct Move {
	int sourceSquare;
	int targetSquare;
	int pieceType;
	MoveType moveType;
	int promotionPiece = QUEEN;
	int score;
};
struct MoveResult {
	bool success;
	int capturedPiece;
	bool capturedColor;
	Bitboard extraState;
};

class Game;
class Bot_V1;
class Bot_V2;
class Bot_V3;
class Bot_V4;
class Bot_SB;
class BoardGL;

class Board {
	friend class Game;
	friend class Bot_V1;
	friend class Bot_V2;
	friend class Bot_V3;
	friend class Bot_V4;
	friend class Bot_SB;
	friend class BoardGL;

protected:
	// Definiciones de máscaras posicionales
	static constexpr Bitboard CENTER_SQUARES = 0x0000001818000000ULL;
	static constexpr Bitboard KING_ZONE_WHITE = 0x0000000000070707ULL;
	static constexpr Bitboard KING_ZONE_BLACK = 0x0707000000000000ULL;
	Bitboard PASSED_PAWN_MASKS[2][64];

	static constexpr Bitboard FILE_MASKS[8] = {
	0x0101010101010101ULL, // Columna a (file 0)
	0x0202020202020202ULL, // Columna b (file 1)
	0x0404040404040404ULL, // Columna c (file 2)
	0x0808080808080808ULL, // Columna d (file 3)
	0x1010101010101010ULL, // Columna e (file 4)
	0x2020202020202020ULL, // Columna f (file 5)
	0x4040404040404040ULL, // Columna g (file 6)
	0x8080808080808080ULL  // Columna h (file 7)
	};

	static constexpr Bitboard RANK_MASKS[8] = {
		0x00000000000000FFULL, // Fila 1 (rank 0)
		0x000000000000FF00ULL, // Fila 2 (rank 1)
		0x0000000000FF0000ULL, // Fila 3 (rank 2)
		0x00000000FF000000ULL, // Fila 4 (rank 3)
		0x000000FF00000000ULL, // Fila 5 (rank 4)
		0x0000FF0000000000ULL, // Fila 6 (rank 5)
		0x00FF000000000000ULL, // Fila 7 (rank 6)
		0xFF00000000000000ULL  // Fila 8 (rank 7)
	};

	// Tablas mágicas para torres (precalculadas)
	Bitboard rookMagics[64];
	Bitboard* rookAttacks[64];  // Puntero a tabla de ataques por casilla
	int rookIndexBits[64];      // Bits de índice para cada casilla

	// Tablas mágicas para alfiles (precalculadas)
	Bitboard bishopMagics[64];
	Bitboard* bishopAttacks[64];// Puntero a tabla de ataques por casilla
	int bishopIndexBits[64];	// Bits de índice para cada casilla


	Bitboard pawnAttacks[2][64];
	Bitboard knightMoves[64];
	Bitboard kingMoves[64];
	Bitboard bishopMasks[64];
	Bitboard rookMasks[64];

	int N;	int M;
	const bool Kramnik;
	int gametype;
	

	ChessState prevStates[500];  // Pila de estados anteriores
	int moveCount = 0;
	ChessState currentState;

	Bitboard getAttackMap(bool color)const {
		return currentState.currentAttacks[color];
	}
	void updateAttackMap() {
		currentState.currentAttacks[0] = calculateAttackMap(0);
		currentState.currentAttacks[1] = calculateAttackMap(1);
	}

	Bitboard boundary;
	void boundaryMask();



public:
	// Constructor principal
	Board(int n) : N(games[n].y), M(games[n].x), Kramnik(games[n].kramnik) {
		gametype = n;
		initBitboards();
		setBitboards();
		initAttackTables();
		updateAttackMap();

	}
	// Constructor de copia
	Board(const Board& other) : N(other.N), M(other.M), Kramnik(other.Kramnik) {
		memcpy(&currentState, &other.currentState, sizeof(ChessState));
	}

	// Destructor
	~Board() = default;

	void printBoard(Bitboard piece)const;

private:

	//logica basica de generacion de movimientos
	Bitboard getPieces(bool color) const {
		return currentState.pieces[color][0] | currentState.pieces[color][1] |
			currentState.pieces[color][2] | currentState.pieces[color][3] |
			currentState.pieces[color][4] | currentState.pieces[color][5];
	}
	void initAttackTables();
	void initPositionalMasks();

	Bitboard generatePawnMoves(int,bool)const;
	Bitboard generateRookMoves(int,bool) const;
	Bitboard generateBishopMoves(int , bool ) const;
	Bitboard generateKnightMoves(int , bool ) const;
	Bitboard generateQueenMoves(int , bool ) const;
	Bitboard generateKingMoves(int,bool ) const;
	Bitboard generateCastlingMoves(bool ) const;

	Bitboard getRookAttacksSlow(int, Bitboard)const;
	Bitboard getRookMask(int)const;

	Bitboard getBishopAttacksSlow(int sq, Bitboard blockers)const;
	Bitboard getBishopMask(int sq) const;




	//metodos para obtener numeros magicos, para el hashing de vimientos torres/alfiles
	Bitboard indexToBitboard(Bitboard, Bitboard);
	Bitboard getAttacksSlow(int sq, Bitboard blockers, bool isRook);

	void initRookMagics();
	void initRookAttacks(int );

	void initBishopMagics();
	void initBishopAttacks(int sq);

	Bitboard findMagic(int sq, bool isRook);
	int calculateMagicQuality(int sq, Bitboard magic, Bitboard mask, int bits, bool isBishop);
	void saveBestMagic(int sq, Bitboard magic, int quality, bool isBishop);
	bool validateMagic(int sq, Bitboard magic, Bitboard mask, int bits, bool isRook);
	void generateMagicsForSquares(bool isRook);




	//funciones asociadas a la verificacion de movimientos
	Bitboard calculateAttackMap(bool color) const;
	Bitboard generateAttacksFrom(int sq, bool ) const;
	Bitboard generateMovesFrom(int, bool) const;
	Bitboard getMovementMap(bool)const;


	MoveResult makeMove( Move&);
	void undoMove();
	MoveType determineMoveType(const Move& move)const;

	bool scanChecks(bool, Bitboard&)const;
	bool scanChecks(bool)const;
	bool scanCheckMate(bool);
	bool canKingMove(bool);
	bool canCaptureAttacker(bool, Bitboard);
	bool canBlockAttacker(bool, Bitboard);

	Bitboard getPath(int, int,int);
	bool checkRays(int,int,int, bool);

	bool isLegalmove(Move&);

	vector<Move> generateAllMoves(bool);
	vector<Move> generateCaptureMoves(bool);


public:
	//bitboard to interface
	void initBitboards();
	void setBitboards();

	int coordToPos(int x, int y) const;
	Bitboard coordToBit(int x, int y) const;
	void bitToCoord(Bitboard b, int& x, int& y) const;
	void removePiece(int x, int y);
	void removeFlag(int x, int y);

	void BitboardSetPiece(int, int, int, bool);
	int BitboardGetColor(int, int)const;
	int BitboardGetColor(int) const;
	int BitboardGetColor(int,ChessState) const;
	int BitboardGetType(int, int)const;
	int BitboardGetType(int) const;

	void BitboardSetFlag(int, int);
	int BitboardGetFlag(int, int);
	void cleanEnpassant();

	void selectCell(int, int);
	void unselectCell(int x, int y);
	bool selected(int x, int y);

	bool BitboardGetTurn() { return currentState.turn; }
	bool isKramnik() const { return Kramnik; }
	int getVariant()const;
	int getSizeY() { return N; }
	int getSizeX() { return M; }
	Bitboard getSelected() { return currentState.selected; }

	//LSB
	static int portable_ctzll(uint64_t b) {
		if (b == 0) return 64;

		// Aislar el bit menos significativo
		uint64_t isolated = b & (~b + 1);

		// Calcular la posición del bit usando logaritmo base 2
		return static_cast<int>(std::log2(isolated));
	}
	//MSB
	static int portable_clzll(uint64_t x) {
		if (x == 0) return 64;
		int n = 0;
		if ((x & 0xFFFFFFFF00000000ULL) == 0) { n += 32; x <<= 32; }
		if ((x & 0xFFFF000000000000ULL) == 0) { n += 16; x <<= 16; }
		if ((x & 0xFF00000000000000ULL) == 0) { n += 8;  x <<= 8; }
		if ((x & 0xF000000000000000ULL) == 0) { n += 4;  x <<= 4; }
		if ((x & 0xC000000000000000ULL) == 0) { n += 2;  x <<= 2; }
		if ((x & 0x8000000000000000ULL) == 0) { n += 1;  x <<= 1; }
		return n;
	}

	bool isCentralSquare(int sq) {
		int x = sq % 8, y = sq / 8;
		return (x >= 2 && x <= 5 && y >= 2 && y <= 5); // Casillas centrales
	}
	
	// Función auxiliar de desplazamiento seguro
	Bitboard shift(Bitboard b, int dir)const{
		if (dir > 0) return b << dir;   // Desplazamiento hacia arriba (blancas)
		else return b >> (-dir);        // Desplazamiento hacia abajo (negras)
	}

	int countBits(Bitboard b) {
		// Método SWAR (5 operaciones para 64 bits)
		b = b - ((b >> 1) & 0x5555555555555555);
		b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333);
		return ((b + (b >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101 >> 56;
	}
};

inline void Board::printBoard(Bitboard board) const {
	cout << "\n\n";
	// Imprimir desde la fila superior (i=7) hasta la inferior (i=0)
	for (int i = 7; i >= 0; i--) {  // <- Cambiar el bucle a decreciente
		for (int j = 0; j < 8; j++) {
			// Calcular el bit correspondiente a la casilla (i, j)
			cout << ((board >> (i * 8 + j)) & 1);
		}
		cout << "\n";
	}
}
inline int Board::getVariant()const {
	int variante = 0;
	if (gametype == 0 || gametype == 1)variante = 0;
	if (gametype == 2 || gametype == 3)variante = 1;
	if (gametype == 4 || gametype == 5)variante = 2;
	return variante;
}
inline void Board::initBitboards() {
	memset(&currentState, 0, sizeof(ChessState));  // Todos los bits a 0
}
inline void Board::setBitboards() {

	initBitboards();
	currentState.turn = WHITE; // El blanco comienza



	switch (getVariant()) {
	case 0:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 6, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Torres
		BitboardSetPiece(0, 7, ROOK, BLACK);    // a8
		BitboardSetPiece(7, 7, ROOK, BLACK);    // h8
		BitboardSetPiece(0, 0, ROOK, WHITE);    // a1
		BitboardSetPiece(7, 0, ROOK, WHITE);    // h1
		// Alfiles
		BitboardSetPiece(2, 7, BISHOP, BLACK);  // c8
		BitboardSetPiece(5, 7, BISHOP, BLACK);  // f8
		BitboardSetPiece(2, 0, BISHOP, WHITE);  // c1
		BitboardSetPiece(5, 0, BISHOP, WHITE);  // f1
		// Caballos
		BitboardSetPiece(1, 7, KNIGHT, BLACK);  // b8
		BitboardSetPiece(6, 7, KNIGHT, BLACK);  // g8
		BitboardSetPiece(1, 0, KNIGHT, WHITE);  // b1
		BitboardSetPiece(6, 0, KNIGHT, WHITE);  // g1
		// Reinas
		BitboardSetPiece(3, 7, QUEEN, BLACK);   // d8
		BitboardSetPiece(3, 0, QUEEN, WHITE);   // d1
		// Reyes
		BitboardSetPiece(4, 7, KING, BLACK);    // e8
		BitboardSetPiece(4, 0, KING, WHITE);    // e1
		currentState.castlingRights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
		break;
	case 1:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 6, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Torres
		BitboardSetPiece(3, 7, ROOK, BLACK);
		BitboardSetPiece(3, 0, ROOK, WHITE);
		// Alfiles
		BitboardSetPiece(1, 7, BISHOP, BLACK);
		BitboardSetPiece(1, 0, BISHOP, WHITE);
		// Caballos
		BitboardSetPiece(2, 7, KNIGHT, BLACK);
		BitboardSetPiece(2, 0, KNIGHT, WHITE);
		// Reyes
		BitboardSetPiece(0, 7, KING, BLACK);
		BitboardSetPiece(0, 0, KING, WHITE);
		break;
	case 2:
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 3, PAWN, BLACK);
		}
		for (int x = 0; x < M; x++) {
			BitboardSetPiece(x, 1, PAWN, WHITE);
		}
		// Reinas
		BitboardSetPiece(1, 4, QUEEN, BLACK);
		BitboardSetPiece(1, 0, QUEEN, WHITE);
		// Torres
		BitboardSetPiece(3, 4, ROOK, BLACK);
		BitboardSetPiece(3, 0, ROOK, WHITE);
		BitboardSetPiece(0, 4, ROOK, BLACK);
		BitboardSetPiece(0, 0, ROOK, WHITE);
		// Reyes
		BitboardSetPiece(2, 4, KING, BLACK);
		BitboardSetPiece(2, 0, KING, WHITE);

		break;
	}

}

inline int Board::coordToPos(int x, int y) const {
	return  (y * 8 + x); // Ej: (2,3) ? 1 << 26
}
inline Bitboard Board::coordToBit(int x, int y) const {
	return 1ULL << (y * 8 + x); // Ej: (2,3) ? 1 << 26
}
inline void Board::bitToCoord(Bitboard b, int& x, int& y) const {
	int pos = portable_ctzll(b); // Posición del bit activo
	x = pos % 8;
	y = pos / 8;
}
inline void Board::removePiece(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	for (int color = 0; color < 2; color++) {
		for (int type = 0; type < 6; type++) { // Saltar EMPTY_CELL
			if (currentState.pieces[color][type] & mask) {
				currentState.pieces[color][type] &= ~mask;
				currentState.occupancy &= ~mask;
				return;
			}
		}
	}
}
inline void Board::removeFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	if (currentState.enPassant & mask) {
		currentState.enPassant &= ~mask;
		currentState.occupancy &= ~mask;
		return;
	}
}


inline int Board::BitboardGetColor(int x, int y) const {
	Bitboard mask = coordToBit(x, y);
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
inline int Board::BitboardGetColor(int sq) const {
	Bitboard mask = 1ULL << sq;
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
inline int Board::BitboardGetColor(int sq, ChessState state) const {
	Bitboard mask = 1ULL << sq;
	int color = 2;

	for (int c = 0; c < 2; c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (state.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
inline int Board::BitboardGetType(int x, int y) const {
	Bitboard mask = coordToBit(x, y);
	int type = 6;

	for (int c = 0; c < 2; ++c) {
		for (int t = 0; t < 6; ++t) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return t;
			}
		}
	}return type;
}
inline int Board::BitboardGetType(int sq) const {
	Bitboard mask = 1ULL << sq;
	int type = 6;

	for (int c = 0; c < 2; ++c) {
		for (int t = 0; t < 6; ++t) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return t;
			}
		}
	}return type;
}
inline void Board::BitboardSetPiece(int x, int y, int pieceType, bool color) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier pieza existente
	removePiece(x, y);

	// Actualizar bitboards específicos
	currentState.pieces[color][pieceType] |= mask;
	currentState.occupancy |= mask;
}


inline void Board::BitboardSetFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier flag
	removeFlag(x, y);

	// Actualizar bitboards específicos
	currentState.enPassant |= mask;

}
inline int Board::BitboardGetFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);
	int flag = 0;

	if (currentState.enPassant & mask) flag = 1;

	return flag;
}

inline void Board::selectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier flag
	unselectCell(x, y);

	// Actualizar bitboards específicos
	currentState.selected |= mask;

}
inline void Board::unselectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	if (currentState.selected & mask) {
		currentState.selected &= ~mask;

	}
}
inline bool Board::selected(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	bool s = 0;
	if (currentState.selected & mask)s = 1;
	return s;
}

inline void Board::cleanEnpassant() {

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {

			removeFlag(i, j);
		}
	}


}

#endif
