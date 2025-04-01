#include "board.h"
#include <iostream>
#include <math.h>


#define PAWN Object::PAWN
#define ROOK Object::ROOK
#define BISHOP Object::BISHOP
#define KNIGHT Object::KNIGHT
#define QUEEN Object::QUEEN
#define KING Object::KING
#define EMPTY_CELL Object::EMPTY_CELL
#define WHITE Object::WHITE 
#define BLACK Object::BLACK
#define NONE Object::NONE

int Board::piecePunctuation(Object::type_t type) {
	switch (type)
	{
	case PAWN:

		return 1;
		break;

	case ROOK:

		return 5;
		break;

	case BISHOP:

		return 3;
		break;

	case KNIGHT:

		return 3;
		break;

	case QUEEN:

		return 9;
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

int Board::EvaluateGame() {
	int score = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (getTab()[i][j].getColor() == bot ) {

				score += piecePunctuation(getTab()[i][j].getType());

			}
			else if(getTab()[i][j].getColor() == player){

				score -= piecePunctuation(getTab()[i][j].getType());
			}
		}
	}
	return score;
}

//añadida escalera de punteros para la correcta implementación del algoritmo alpha beta prunning
//minmax alpha beta prunning da resultados consistentes 
//falta almacenar la ultima capa de movimientos para realizar un movimiento aleatorio de entre los que mejor resultado dan
int Board::minimax(int depth,int *alpha,int *beta, bool maximizingPlayer) {
	
	Object::color_t color;
	int maxEval = -10000, minEval = 10000;
	int alpha1 = *alpha,  beta1 = *beta;

	if (maximizingPlayer) color = bot;
	else color = player;
	
	if (depth == 0 || scanCheckMate(color)) {
		//cout << "\n" << EvaluateGame();
		return EvaluateGame();
	}

	else {
		for (int i = 0; i < N; i++) {//busca todas las piezas del color del turno en el tablero
			for (int j = 0; j < N; j++) {

				if (getTab()[i][j].getColor() == color) {

					//escanea todos los movimientos de esa pieza
					//itera la funcion minmax
					switch (getTab()[i][j].getType())
					{
					case  PAWN:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && pawnMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
									bool b_ep = false;
									bool w_ep = false;
									Object::type_t w_ep_t, b_ep_t;
									Object::color_t w_ep_c, b_ep_c;
									//añadir en passant
									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									//almacenar en passant
									if (k - 1 >= 0) {
										w_ep_t = getTab()[k - 1][l].getType();
										w_ep_c = getTab()[k - 1][l].getColor();
									}

									if (k + 1 < N) {
										b_ep_t = getTab()[k + 1][l].getType();
										b_ep_c = getTab()[k + 1][l].getColor();
									}


									if (getTab()[k][l].getFlag() == Object::WHITE_EP) {
										w_ep = true;
										getTab()[k - 1][l].setCell(k - 1, l, EMPTY_CELL, NONE);
									}
									if (getTab()[k][l].getFlag() == Object::BLACK_EP) {
										b_ep = true;
										getTab()[k + 1][l].setCell(k + 1, l, EMPTY_CELL, NONE);
									}

									getTab()[k][l].setCell(k, l, PAWN, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src



									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1,&alpha1,&beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										
									}


									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, PAWN, color);//src

									if (w_ep) {
										getTab()[k - 1][l].setCell(k - 1, l, w_ep_t, w_ep_c);
									}
									if (b_ep) {
										getTab()[k + 1][l].setCell(k + 1, l, b_ep_t, b_ep_c);
									}

									//clear generated en passant flags
									if (abs(i - k) == 2 && color == WHITE)getTab()[k + 1][l].setFlag(k + 1, l, Object::NO_FLAG);
									if (abs(i - k) == 2 && color == BLACK)getTab()[k - 1][l].setFlag(k - 1, l, Object::NO_FLAG);

								}
							}
						}
						break;

					case  ROOK:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && rookMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon

									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									getTab()[k][l].setCell(k, l, ROOK, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src

									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
									}
									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, ROOK, color);//src

								}
							}
						}
						break;

					case  BISHOP:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && bishopMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon

									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									getTab()[k][l].setCell(k, l, BISHOP, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src

									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
									}
									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, BISHOP, color);//src

								}
							}
						}
						break;

					case  KNIGHT:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && knigthMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon

									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									getTab()[k][l].setCell(k, l, KNIGHT, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src

									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
									}
									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, KNIGHT, color);//src

								}
							}
						}
						break;

					case  QUEEN:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && queenMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon

									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									getTab()[k][l].setCell(k, l, QUEEN, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src

									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
									}
									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, QUEEN, color);//src

								}
							}
						}
						break;

					case  KING:

						for (int k = 0; k < N; k++) {
							for (int l = 0; l < N; l++) {
								if (getTab()[k][l].getColor() != color && kingMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon

									auto destination_t = getTab()[k][l].getType();
									auto destination_c = getTab()[k][l].getColor();
									getTab()[k][l].setCell(k, l, KING, color);//dest
									getTab()[i][j].setCell(i, j, EMPTY_CELL, NONE);//src

									//check if result in checkmate 
									if (!scanChecks(color) && findKing(color)) {
										if (maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, false);
											maxEval = max(maxEval, eval);
											alpha1 = max(alpha1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
										if (!maximizingPlayer) {
											int eval = minimax(depth - 1, &alpha1, &beta1, true);
											minEval = min(minEval, eval);
											beta1 = min(beta1, eval);
											//cout << "\nalpha  beta  :  " << alpha1 << "  :  " << beta1;
										}
									}
									//undo
									getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
									getTab()[i][j].setCell(i, j, KING, color);//src

								}
							}
						}
						break;

					case  EMPTY_CELL:

						std::cout << "Si la casilla esta vacía no debería llegar aqui" << std::endl;
						break;

					default:

						std::cerr << "Ha ocurrido un error" << std::endl;
						break;
					}

					if (beta1 <= alpha1)  break;
				}
			}
		}
		if (maximizingPlayer) { /*if (depth >= 1)cout << "\nbot moving depth :" << depth << "    maxeval :" << maxEval;*/ return maxEval; }
		if (!maximizingPlayer ) { /*if (depth >= 1)cout << "\nbot moving depth :" << depth << "    mineval :" << minEval;*/ return minEval; }
	}
}
