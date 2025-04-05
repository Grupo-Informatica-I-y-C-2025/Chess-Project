#pragma once
#include "Game.h"
#include <random>
#include "board.h"

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



int Game::piecePunctuation(Object::type_t type) {
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

int Game::EvaluateGame(Board* board) {
	int score = 0;

	board->listPieces();
	for (const auto& p : board->board_pieces) {
		if (p.color == bot ) score += piecePunctuation(p.type);
		else if(p.color == Player())score -= piecePunctuation(p.type);
	}
	return score;
}

//añadida escalera de punteros para la correcta implementación del algoritmo alpha beta prunning
//minmax alpha beta prunning da resultados consistentes 
int Game::minimax( int depth, int* n, int* alpha, int* beta, bool maximizingPlayer, vector <movement>& moves) {

	Object::color_t color;
	int maxEval = -10000, minEval = 10000;
	int alpha1 = *alpha, beta1 = *beta;

	if (maximizingPlayer) color = bot;
	else color = Player();

	if (depth == 0) return  EvaluateGame(board);
	else {
		//cout << "\n" << depth;

		for (const auto& move : generateAllMoves(*board, color)) {
			board_piece ep = enPassant(board, move);
			board->getTab()[move.destination.y][move.destination.x].setCell(move.destination.y, move.destination.x, move.source.type, move.source.color);
			board->getTab()[move.source.y][move.source.x].setCell(move.source.y, move.source.x, EMPTY_CELL, NONE);
			

			//check if result in checkmate
			//bool inCheck = scanChecks(board, color);
			if (!scanChecks(board, color)) {
				int eval = minimax( depth - 1, n, &alpha1, &beta1, !maximizingPlayer, moves);

				// [5] Actualizar alpha/beta con logs
				if (maximizingPlayer) {
					maxEval = max(maxEval, eval);
					alpha1 = max(alpha1, eval);
				}
				else {
					minEval = min(minEval, eval);
					beta1 = min(beta1, eval);
				}
				if (*n == depth && eval<10000 && eval > -10000) {
					auto moveD1 = move;
					moveD1.score = eval;
					moves.push_back(moveD1);
				}
			}
			//undo
			unPassant(board, move, ep);
			board->getTab()[move.destination.y][move.destination.x].setCell(move.destination.y, move.destination.x, move.destination.type, move.destination.color);
			board->getTab()[move.source.y][move.source.x].setCell(move.source.y, move.source.x, move.source.type, move.source.color);
			board->listPieces();
			if (beta1 <= alpha1)  break;

		}

		if (maximizingPlayer) { /*if (depth>1)cout << "\nbot moving depth :" << depth << "    maxeval :" << maxEval;*/ return maxEval; }
		if (!maximizingPlayer) { /* if (depth > 1)cout << "\nbot moving depth :" << depth << "    mineval :" << minEval;*/ return minEval; }
	}
}

movement Game::botMove(Object::color_t c) {
	int alpha = -10000, beta = 10000, n = 4; bool a = true;//despues de pruebas, 6 es el limite computable en corto tiempo
	vector <movement> moves;

	if (c == bot)a = true;
	if (c == player)a = false;
	int bestscore = minimax(n, &n, &alpha, &beta, a,  moves);

	//selecciona uno de los mejores movimientos de forma randomizada
	vector <movement> bestmoves;
	for (const auto& m : moves) {
		if ( bestscore == m.score) {
			bestmoves.push_back(m);
		}
	}

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<size_t> dis(0, bestmoves.size() - 1);

	movement bestmove = { bestmoves[dis(gen)] };

	return  bestmove;
}
