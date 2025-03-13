
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
