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
	Board(int n, bool i) :N(n), Kramnik(i) {

		tab = new Piece * [N];
		for (int i = 0; i < N; i++) {
			tab[i] = new Piece[N];			//initially empty
		}
	
	}

	~Board() {
		for (int i = 0; i < N; i++)
			delete[] tab[i];
		delete[] tab;
	}

	int getSize() { return N; }
	Piece** getTab() { return tab; }
	Object::color_t getTurn() { return turn; }

	//reglas de movimiento
	void activate(int xcell_sel, int ycell_sel);
	void registerCall(int xcell_sel, int ycell_sel);
	bool makeMove();
	bool scanCheckMate(Object::color_t turn);
	void setBoard(/*game_type*/);


	//funciones para implementar bot
	//si la partida es con bot
	Object::color_t player = Object::WHITE;
	Object::color_t bot = Object::BLACK;

	int piecePunctuation(Object::type_t);
	int EvaluateGame();
	int minimax(int depth,int*,int*, bool maximizingPlayer);



	private:
	const bool Kramnik;
	bool action;
	int src[2] = { -1,-1 }, dest[2] = { -1,-1 };
	int ky, kx;
	Object::color_t turn = Object::WHITE;//turno inicial

	bool isKramnik() { return Kramnik; }
	void doMove();
	void unselectAll();
	void changeTurn();
	bool findKing(Object::color_t c);

	bool scanChecks(Object::color_t c);

	void enPassant(int dy, int dx);
	void cleanEnpassant(Object::color_t c);



	//movimientos permitidos
	bool pawnMove(int sy, int sx, int dy, int dx);
	bool rookMove(int sy, int sx, int dy, int dx);
	bool bishopMove(int sy, int sx, int dy, int dx);
	bool knigthMove(int sy, int sx, int dy, int dx);
	bool queenMove(int sy, int sx, int dy, int dx);
	bool kingMove(int sy, int sx, int dy, int dx);
};




#endif
