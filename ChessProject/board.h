
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
	Bitboard pieces[2][6] = { {0} };	 // 0:WHITE, 1:BLACK | 0:EMPTY,1:PAWN...6:KING
	Bitboard enPassant =  0 ;       // Casilla de en passant
	Bitboard castlingRights = 0;
	Bitboard selected = 0;
	Bitboard occupancy = 0;			     // Todas las piezas (cache automático)
	bool turn;
};
struct Move {
	int sourceSquare;    // 0-63
	int targetSquare;
	int pieceType;       // PAWN, ROOK, etc
	MoveType moveType;        // NORMAL, EN_PASSANT, CASTLING, PROMOTION
	int promotionPiece = QUEEN; // Solo relevante para promociones
	int score;
};
struct MoveResult {
	bool success;
	int capturedPiece;    // Tipo de pieza capturada
	bool capturedColor;    // Color de pieza capturada
	Bitboard extraState;  // Para guardar info de en passant/roque
};
class Board {

public:

	static constexpr Bitboard FILE_A = 0x0101010101010101;
	static constexpr Bitboard FILE_H = 0x8080808080808080;
	static constexpr Bitboard RANK_1 = 0x00000000000000FFULL; // Fila 1 (bits 0-7)
	static constexpr Bitboard RANK_2 = 0x000000000000FF00ULL;  // Fila 2 (bits 8-15)
	static constexpr Bitboard RANK_3 = 0x0000000000FF0000ULL;  // Fila 3 (bits 16-23)
	static constexpr Bitboard RANK_4 = 0x00000000FF000000ULL;  // Fila 4 (bits 24-31)
	static constexpr Bitboard RANK_5 = 0x000000FF00000000ULL;  // Fila 5 (bits 32-39)
	static constexpr Bitboard RANK_6 = 0x0000FF0000000000ULL;  // Fila 6 (bits 40-47)
	static constexpr Bitboard RANK_7 = 0x00FF000000000000ULL;  // Fila 7 (bits 48-55)
	static constexpr Bitboard RANK_8 = 0xFF00000000000000ULL;  // Fila 8 (bits 56-63)

	// Tablas mágicas para torres (precalculadas)
	Bitboard rookMagics[64];
	Bitboard* rookAttacks[64];  // Puntero a tabla de ataques por casilla
	int rookIndexBits[64];      // Bits de índice para cada casilla

	// Tablas mágicas para alfiles (precalculadas)
	Bitboard bishopMagics[64];
	Bitboard* bishopAttacks[64];// Puntero a tabla de ataques por casilla
	int bishopIndexBits[64];	// Bits de índice para cada casilla


	Bitboard pawnAttacks[2][64];    // [color][casilla]
	Bitboard knightMoves[64];
	Bitboard kingMoves[64];
	Bitboard bishopMasks[64];
	Bitboard rookMasks[64];

	int N;	int M;	//tablero NxN
	const bool Kramnik;

	ChessState prevStates[256];  // Pila de estados anteriores
	int moveCount = 0;



	ChessState currentState;


	// Constructor principal
	Board(int n, int m, bool k) : N(n), M(m), Kramnik(k) {
		initBitboards();    // Inicializar todos los bitboards a 0
		setBitboards();     // Inicializar los bitboards con la configuración deseada
		initAttackTables(); // Inicializar los bitboards de máscaras de ataque

	}
	// Constructor de copia
	Board(const Board& other) : N(other.N), M(other.M), Kramnik(other.Kramnik) {
		memcpy(&currentState, &other.currentState, sizeof(ChessState));
	}

	// Destructor (ya no necesita liberar memoria manualmente)
	~Board() = default;


	bool isKramnik() const { return Kramnik; }

	void printBoard( Bitboard piece)const;
	
	//move generation
	Bitboard getPieces(bool color) const {
		return currentState.pieces[color][0] | currentState.pieces[color][1] |
			currentState.pieces[color][2] | currentState.pieces[color][3] |
			currentState.pieces[color][4] | currentState.pieces[color][5];
	}
	void initAttackTables();
	
	
	

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
	Bitboard generateAttacksFrom(int sq, bool color) const;
	vector<Move> generateAllMoves(bool) ;
	
	bool isLegalmove( Move&) ;

	MoveResult makeMove( Move&);
	void undoMove();
	MoveType determineMoveType(const Move& move)const;


	bool scanChecks(bool, Bitboard&);
	bool scanChecks(bool);
	bool scanCheckMate(bool);
	bool canKingMove(bool);
	bool canCaptureAttacker(bool, Bitboard);
	bool canBlockAttacker(bool, Bitboard);


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


	int countBits(Bitboard b) {
		// Método SWAR (5 operaciones para 64 bits)
		b = b - ((b >> 1) & 0x5555555555555555);
		b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333);
		return ((b + (b >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101 >> 56;
	}
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
	Bitboard shift(Bitboard b, int dir) const {
		if (dir > 0) return b << dir;   // Desplazamiento hacia arriba (blancas)
		else return b >> (-dir);        // Desplazamiento hacia abajo (negras)
	}

};




#endif

