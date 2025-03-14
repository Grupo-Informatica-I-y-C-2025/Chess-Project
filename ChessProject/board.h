
#ifndef __BOARD_H__
#define __BOARD_H__

#include <iostream>
#include "piece.h"

using namespace std;

class Board {

protected:
	const int N;		//NxN board
	Piece** tab;

public:
	Board(int n) :N(n) {

		tab = new Piece * [N];
		for (int i = 0; i < N; i++) {
			tab[i] = new Piece[N];			//initially empty
		}
		//set initial position
		for (int j = 0; j < N; j++) {
			tab[1][j].setCell(1, j, Piece::PAWN, Piece::BLACK);
		}
		for (int j = 0; j < N; j++) {
			tab[N - 2][j].setCell(N-2, j, Piece::PAWN, Piece::WHITE);
		}
		tab[0][0].setCell(0, 0, Piece::ROOK, Piece::BLACK);
		tab[0][7].setCell(0, 7, Piece::ROOK, Piece::BLACK);
		tab[7][0].setCell(7, 0, Piece::ROOK, Piece::WHITE);
		tab[7][7].setCell(7, 7, Piece::ROOK, Piece::WHITE);

		tab[0][2].setCell(0, 2, Piece::BISHOP, Piece::BLACK);
		tab[0][5].setCell(0, 5, Piece::BISHOP, Piece::BLACK);
		tab[7][2].setCell(7, 2, Piece::BISHOP, Piece::WHITE);
		tab[7][5].setCell(7, 5, Piece::BISHOP, Piece::WHITE);

		tab[0][1].setCell(0, 1, Piece::KNIGTH, Piece::BLACK);
		tab[0][6].setCell(0, 6, Piece::KNIGTH, Piece::BLACK);
		tab[7][1].setCell(7, 1, Piece::KNIGTH, Piece::WHITE);
		tab[7][6].setCell(7, 6, Piece::KNIGTH, Piece::WHITE);

		tab[7][4].setCell(7, 4, Piece::QUEEN, Piece::WHITE);
		tab[0][4].setCell(0, 4, Piece::QUEEN, Piece::BLACK);

		tab[7][3].setCell(7, 3, Piece::KING, Piece::WHITE);
		tab[0][3].setCell(0, 3, Piece::KING, Piece::BLACK);

	}

	~Board() {
		for (int i = 0; i < N; i++)
			delete[] tab[i];
		delete[] tab;
	}

	int getSize() { return N; }
	Piece** getTab() { return tab; }
};




#endif
