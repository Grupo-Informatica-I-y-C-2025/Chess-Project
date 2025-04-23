
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
 NONE = 6,
};
enum PieceColor {
	WHITE = 0,
	BLACK = 1,
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
class Board {

public:
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



	// Constructor principal
	Board(int n, int m, bool k) : N(n), M(m), Kramnik(k) {
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


	bool isKramnik() const { return Kramnik; }

	void printBoard( Bitboard piece)const;
	


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
	Bitboard getMovementMap(bool);


	MoveResult makeMove( Move&);
	void undoMove();
	MoveType determineMoveType(const Move& move)const;

	bool scanChecks(bool, Bitboard&);
	bool scanChecks(bool);
	bool scanCheckMate(bool);
	bool scanDraw();
	bool canKingMove(bool);
	bool canCaptureAttacker(bool, Bitboard);
	bool canBlockAttacker(bool, Bitboard);

	Bitboard getPath(int, int,int);
	bool checkRays(int,int,int, bool);

	bool isLegalmove(Move&);

	vector<Move> generateAllMoves(bool);
	vector<Move> generateCaptureMoves(bool);
	vector<Move> generateCheckMoves(bool);
	/*vector<Move> generateNormalMoves(bool);
	vector<Move> generateEscapeMoves(bool, Bitboard);
	vector<Move> generateKingMove(bool);
	vector<Move> generateCaptureAttacker(bool, Bitboard);
	vector<Move> generateBlockAttacker(bool, Bitboard);*/



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
	bool BitboardChangeTurn() { currentState.turn = (currentState.turn) ? 0 : 1; return currentState.turn;}

	int getSizeY() { return N; }
	int getSizeX() { return M; }

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



#endif
