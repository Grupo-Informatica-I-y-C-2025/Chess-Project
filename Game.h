#pragma once
#include "board.h"

struct BoardState {
	Piece src_piece;
	Piece dest_piece;
	//vector<board_piece> pieces; // Copia de seguridad de las piezas
	//int ky, kx; // Posición del rey (si se mueve)
	Object::flag_t en_passant_flag; // Para captura al paso
};

class Game {


	
protected:
	Board* board;
	int N;
	int M;

public:
	Game(Board* pb) :board(pb) {
		N = pb->getSizeY();
		M = pb->getSizeX();		//Grid NxN
		board->setBoard();
		updateAttackTables();    // inicializa attack tables
	}

	//reglas de movimiento
	void activate(int xcell_sel, int ycell_sel);
	void registerCall(int xcell_sel, int ycell_sel);
	void makeMove();
	bool scanCheckMate(Object::color_t turn);

	//mover a private mas adelante
	void updateAttackTables();
	vector<vector<bool>> whiteAttackTable;
	vector<vector<bool>> blackAttackTable;

	//funciones para implementar bot
	//si la partida es con bot
	
	board_piece findPiece( int x, int y);
	vector<movement> generateAllMoves(Board& , Object::color_t);

	int piecePunctuation(Object::type_t);
	int EvaluateGame(Board* board);
	int minimax(int depth, int*, int*, int*, bool maximizingPlayer, vector <movement>&);
	movement botMove(Object::color_t c);

	void unmakevirtualMove(Board* board, const movement& m, const BoardState& state);
	BoardState makevirtualMove(Board* board, const movement& m);

	//turn and player things
	Object::color_t player = Object::WHITE;
	Object::color_t bot = Object::BLACK;
	Object::color_t Player() { return player; }
	Object::color_t getTurn() { return turn; }
	bool isgame_over() { if (scanCheckMate(turn)) game_over = true; return game_over; }
	void runAutoGame() {
		while (!isgame_over()) {
			makeMove(); 
		}
	}

private:

	bool game_over = false;
	bool action = false;
	int src[2] = { -1,-1 }, dest[2] = { -1,-1 };
	Object::color_t turn = Object::WHITE;//turno inicial
	bool doMove(Board* board, movement& move);
	void unselectAll();
	void changeTurn();
	bool findKing(Board* board, Object::color_t c);
	
	
	bool scanChecks(Board* board, Object::color_t c);
	
	void calculatePieceAttacks(int x, int y);


	board_piece enPassant(Board* board,const movement& move);
	void unPassant(Board* board, const movement& move, const board_piece& ep);
	void cleanEnpassant(Board* board);

};
