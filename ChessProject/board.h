#ifndef __BOARD_H__
#define __BOARD_H__

#pragma once
#include <iostream>
#include "piece.h"
#include <vector>


using namespace std;

//clase trivial board_piece para simplificar el codigo
struct board_piece {
	int y;
	int x;
	Object::color_t color;
	Object::type_t type;
};
struct movement {
	board_piece source;
	board_piece destination;
	int score;
};
class Board {


	int N;	int M;	//tablero NxN
	
	const bool Kramnik;
protected:
	
	Piece** tab;

public:

	//creación de la clase tablero(board)
    Board(int n,int m, bool i) : N(n),M(m), Kramnik(i) {
        tab = new Piece * [N];
        for (int i = 0; i < N; i++) {
            tab[i] = new Piece[N];  //inicialmente vacío
        }
    }

    // Constructor de copia (para clonar un Board)
    Board(const Board& other) : N(other.N), M(other.M), Kramnik(other.Kramnik), board_pieces(other.board_pieces) {
        tab = new Piece * [N];
        for (int i = 0; i < N; i++) {
            tab[i] = new Piece[M];
            for (int j = 0; j < M; j++) {
                tab[i][j] = other.tab[i][j];  // Copiar cada pieza
            }
        }
    }

    // Destructor para liberar memoria correctamente
    ~Board() {
        for (int i = 0; i < N; i++) {
            delete[] tab[i];
        }
        delete[] tab;
    }



	bool isKramnik() const { return Kramnik; }

	vector<board_piece> board_pieces;
	void listPieces();


	//funciones de validez de los movimientos
	bool validateRayMove(movement, bool);
	bool pawnMove(movement &);
	bool rookMove(movement &);
	bool bishopMove( movement &);
	bool knigthMove(movement &);
	bool queenMove(movement &);
	bool kingMove(movement &);
	bool isLegalmove( movement &);








	int getSizeY() { return N; }
	int getSizeX() { return M; }
	Piece** getTab() { return tab; }
	int ky, kx;

	void setBoard();

};




#endif
