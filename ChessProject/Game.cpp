#include "Game.h"


//mover a boardGL
void Game::unselectAll() {
	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			board->unselectCell(i,j);
		}
	}
}
void Game::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < M)) {
		if (board->selected(ycell_sel, xcell_sel))
		{
			board->unselectCell(ycell_sel, xcell_sel);
			movement.sourceSquare = -1, movement.targetSquare = -1;
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == board->BitboardGetTurn())
		{
			unselectAll();
			board->selectCell(ycell_sel, xcell_sel);
			//board->printBoard(board->generateMovesFrom(board->coordToPos(ycell_sel, xcell_sel),board->BitboardGetColor(ycell_sel, xcell_sel)));
			if(!board->isKramnik())movement.sourceSquare = -1;
			registerCall(xcell_sel, ycell_sel);
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == !board->BitboardGetTurn() || board->BitboardGetColor(ycell_sel, xcell_sel) == 2) {
			unselectAll();
			registerCall(xcell_sel, ycell_sel);
		}
	}

}
void Game::registerCall(int xcell_sel, int ycell_sel) {
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < M)) {
		if (movement.sourceSquare<0)
		{
			movement.sourceSquare = xcell_sel * 8 + ycell_sel;
		}
		else {
			movement.targetSquare = xcell_sel * 8 + ycell_sel;
		}
	}
}

void Game::playTurn() {

	bool turn = board->BitboardGetTurn();

	if (!game_over && turn == bot && !generated) {
		movement = black_bot.botMove(turn);
		cout << "\nmove :" << movement.sourceSquare % 8 << movement.sourceSquare / 8 << " " << movement.targetSquare % 8 << movement.targetSquare / 8;
		generated = true;
	}
	/*if (!game_over && turn == player && !generated) {
		movement = white_bot.botMove(turn);
		//cout << "\nmove :" << movement.sourceSquare % 8 << movement.sourceSquare / 8 << " " << movement.targetSquare % 8 << movement.targetSquare / 8;
		generated = true;
	}*/

	if (movement.sourceSquare >= 0 && movement.targetSquare >= 0) {
		movement.pieceType = board->BitboardGetType(movement.sourceSquare);
		if (board->isLegalmove(movement)) {
			board->makeMove(movement);
			generated = false;
			scanEndGame(!turn);
			if (!game_over && board->scanChecks(!turn)) cout << "\ncheck";
		}
		else {
			cout << "\nIllegal move";
		}
		//reset
		
		movement.sourceSquare = -1, movement.targetSquare = -1;
	}
}


void Game::scanEndGame(bool color) {
	vector<Move> moves = board->generateAllMoves(color);
	if (moves.size() == 0) {
		if (board->scanChecks(color)) {
			cout << "\ncheckmate";
			game_over = true;
			return;
		}
		else {
			cout << "\ndraw";
			game_over = true;
			return;
		}
	}
	if (board->moveCount == 499) {
		game_over = true;
		cout << "\noverride stack memory";
	}
}

