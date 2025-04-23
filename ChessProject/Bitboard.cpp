#pragma once
#include "board.h"


#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5


#define WHITE 0
#define BLACK 1


void Board::printBoard(Bitboard board) const {
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

void Board::initBitboards() {
	memset(&currentState, 0, sizeof(ChessState));  // Todos los bits a 0
}
void Board::setBitboards() {

	initBitboards();
	
	//set initial position
	// Peones negros (fila 1, que es y=1 en coordenadas de tablero)
	for (int x = 0; x < M; x++) {
		BitboardSetPiece(x, 6, PAWN, BLACK);
	}
	// Peones blancos (fila 6, que es y=6 en coordenadas de tablero)
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
	currentState.turn = WHITE; // El blanco comienza
	cout << "\nBoard set correctly";
}

 int Board::coordToPos(int x, int y) const {
	return  (y * 8 + x); // Ej: (2,3) ? 1 << 26
}
 Bitboard Board::coordToBit(int x, int y) const {
	return 1ULL << (y * 8 + x); // Ej: (2,3) ? 1 << 26
}
 void Board::bitToCoord(Bitboard b, int& x, int& y) const {
	int pos = portable_ctzll(b); // Posición del bit activo
	x = pos % 8;
	y = pos / 8;
}
 void Board::removePiece(int x, int y) {
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
 void Board::removeFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	if (currentState.enPassant & mask) {
		currentState.enPassant &= ~mask;
		currentState.occupancy &= ~mask;
		return;
	}
}


 int Board::BitboardGetColor(int x, int y) const {
	Bitboard mask = coordToBit(x, y);
	int color = 2;

	for (int c = 0; c < 2;c++) {
		for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			if (currentState.pieces[c][t] & mask) {
				return c;
			}
		}
	}return color;
}
 int Board::BitboardGetColor(int sq) const {
	 Bitboard mask = 1ULL << sq;
	 bool color = 2;

	 for (int c = 0; c < 2; c++) {
		 for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			 if (currentState.pieces[c][t] & mask) {
				 return c;
			 }
		 }
	 }return color;
 }
 int Board::BitboardGetColor(int sq,ChessState state) const {
	 Bitboard mask = 1ULL << sq;
	 bool color = 2;

	 for (int c = 0; c < 2; c++) {
		 for (int t = 0; t < 6; t++) { // EMPTY_CELL no se almacena
			 if (state.pieces[c][t] & mask) {
				 return c;
			 }
		 }
	 }return color;
 }
 int Board::BitboardGetType(int x, int y) const {
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
 int Board::BitboardGetType(int sq) const {
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
 void Board::BitboardSetPiece(int x, int y, int pieceType, bool color) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier pieza existente
	removePiece(x, y);

	// Actualizar bitboards específicos
	currentState.pieces[color][pieceType] |= mask;
	currentState.occupancy |= mask;
}


 void Board::BitboardSetFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier flag
	removeFlag(x, y);

	// Actualizar bitboards específicos
	currentState.enPassant |= mask;

}
 int Board::BitboardGetFlag(int x, int y) {
	Bitboard mask = coordToBit(x, y);
	int flag = 0;

	if (currentState.enPassant & mask) flag = 1;

	return flag ;
}

 void Board::selectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Limpiar cualquier flag
	unselectCell(x, y);

	// Actualizar bitboards específicos
	currentState.selected |= mask;

}
 void Board::unselectCell(int x, int y) {
	Bitboard mask = coordToBit(x, y);

	// Buscar en todos los tipos y colores
	if (currentState.selected & mask) {
		currentState.selected &= ~mask;

	}
}
 bool Board::selected(int x, int y)  {
	Bitboard mask = coordToBit(x, y);

	bool s = 0;
	if (currentState.selected & mask)s = 1;	
	return s;
}

 void Board::cleanEnpassant() {

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {

			removeFlag(i, j);
		}
	}


}
