
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cstring>

#include <bitset>
#include <climits>

#include <random>

#include <omp.h>

#include <fstream>
#include <algorithm>
#include "fileUtils.h"


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
	Bitboard selected = 0;
	bool turn;
	
	int8_t pieceType[64];
	int8_t pieceColor[64];

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
	bool ischeck=false;

};
struct MoveResult {
	bool success = false;

	int targetSquare;
	// Para revertir pieza capturada
	int capturedPiece = EMPTY_CELL;
	int capturedColor = NONE;
};

// Definiciones de m�scaras posicionales
static constexpr Bitboard CENTER_SQUARES = 0x0000001818000000ULL;
static constexpr Bitboard KING_ZONE_WHITE = 0x0000000000070707ULL;
static constexpr Bitboard KING_ZONE_BLACK = 0x0707000000000000ULL;

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


class Game;
class Evaluation;
class Generator;
class Bot_V4;
class BoardGL;


//LSB
inline int portable_ctzll(uint64_t b) {
	if (b == 0) return 64;

	// Aislar el bit menos significativo
	uint64_t isolated = b & (~b + 1);

	// Calcular la posici�n del bit usando logaritmo base 2
	return static_cast<int>(log2(isolated));
}

inline bool isCentralSquare(int sq) {
	int x = sq % 8, y = sq / 8;
	return (x >= 2 && x <= 5 && y >= 2 && y <= 5); // Casillas centrales
}


inline Bitboard shift(Bitboard b, int dir){
	if (dir > 0) return b << dir;   // Desplazamiento hacia arriba (blancas)
	else return b >> (-dir);        // Desplazamiento hacia abajo (negras)
}

inline int countBits(Bitboard b) {
	// M�todo SWAR (5 operaciones para 64 bits)
	b = b - ((b >> 1) & 0x5555555555555555);
	b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333);
	return ((b + (b >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101 >> 56;
}

inline int portable_clzll(uint64_t x) {
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

class Board {
	friend class Game;
	friend class Evaluation;
	friend class Generator;
	friend class Bot_V4;
	friend class BoardGL;

protected:
	//estructuras de datos asociadas al cacheado
	Bitboard PASSED_PAWN_MASKS[2][64];

	Bitboard rookMagics[64];
	Bitboard* rookAttacks[64];  // Puntero a tabla de ataques por casilla
	int rookIndexBits[64];      // Bits de �ndice para cada casilla

	Bitboard bishopMagics[64];
	Bitboard* bishopAttacks[64];// Puntero a tabla de ataques por casilla
	int bishopIndexBits[64];	// Bits de �ndice para cada casilla


	Bitboard pawnAttacks[2][64];
	Bitboard knightMoves[64];
	Bitboard kingMoves[64];
	Bitboard bishopMasks[64];
	Bitboard rookMasks[64];

	Bitboard currentAttacks[2];

	ChessState prevStates[500];  // Pila de estados anteriores
	int moveCount = 0;
	ChessState currentState;
 
	Bitboard getAttackMap(bool color)const {
		return currentAttacks[color];
	}

	Bitboard boundary;
	void boundaryMask();

	void initCached();
	void initBitboards();
	void setBitboards();	





	//datos de inicializacion del tablero
	int N;	int M;
	const bool Kramnik;
	int gametype;



public:

	vector<Move> moveHistory;
	// Constructores
	Board(int n) : N(games[n].y), M(games[n].x), Kramnik(games[n].kramnik) {
		gametype = n;
		setBitboards();
		initAttackTables();

	}
	
	Board(const Board& other) : N(other.N), M(other.M), Kramnik(other.Kramnik) {
		memcpy(&currentState, &other.currentState, sizeof(ChessState));
	}

	
	~Board() = default;

private:

	//logica basica de generacion de movimientos
	Bitboard getPieces(bool color) const {
		return currentState.pieces[color][0] | currentState.pieces[color][1] |
			currentState.pieces[color][2] | currentState.pieces[color][3] |
			currentState.pieces[color][4] | currentState.pieces[color][5];
	}
	void initAttackTables();
	void initPositionalMasks();
	void initializePassedPawnMasks();

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
	bool validateMagic(int sq, Bitboard magic, Bitboard mask,int bits, int tableSize,bool isBishop);
	void generateMagicsForSquares(bool isRook);




	//funciones asociadas a la verificacion de movimientos

	bool scanChecks(bool, Bitboard&)const;
	bool scanChecks(bool)const;
	bool scanCheckMate(bool)const;
	bool scanRepetition()const;
	bool canKingMove(bool)const;
	bool canCaptureAttacker(bool, Bitboard)const;
	bool canBlockAttacker(bool, Bitboard)const;

	Bitboard getPath(int, int,int)const;
	bool checkRays(int,int,int, bool)const;


	//funciones asociadas a la generacion de m ovimiento
	vector<Move> generateAllMoves(bool);
	vector<Move> generateCaptureMoves(bool);
	vector<Move> generateQuiescientMoves(bool);
	vector<Move> generateCheckMoves(bool);

public:
//funciones asociadas a ficheros y codificacion
	bool saveGame(const string& );
	bool loadGame(const string& );
	const ChessState* getState(int count) const;
	string encodeState(ChessState) const;
	bool decodeState(const string&);


	//funciones de operaciones basicas con bits

	Bitboard coordToBit(int x, int y) const;
	void removePiece(int x, int y);
	void removeFlag(int x, int y);

	int piecePunctuation(int)const;

	void BitboardSetPiece(int, int, int, bool);
	int BitboardGetColor(int, int)const;
	int BitboardGetColor(int) const;
	int BitboardGetColor(int,ChessState) const;
	int BitboardGetType(int, int)const;
	int BitboardGetType(int) const;
	int BitboardGetType(int, ChessState) const;
	Bitboard attackersTo(int , bool ) const;

	void BitboardSetFlag(int, int);
	int BitboardGetFlag(int, int);

	void selectCell(int, int);
	void unselectCell(int x, int y);
	void unselectAll();
	bool selected(int x, int y);

	bool BitboardGetTurn() { return currentState.turn; }
	bool isKramnik() const { return Kramnik; }
	int getVariant()const;
	int getSizeY() { return N; }
	int getSizeX() { return M; }
	Bitboard getSelected() { return currentState.selected; }
	void printBoard(Bitboard piece)const;

};


