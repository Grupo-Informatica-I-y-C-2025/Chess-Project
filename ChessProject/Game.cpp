#include "Game.h"


//mover a boardGL
void Game::unselectAll() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			board->unselectCell(i,j);
		}
	}
}
void Game::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < N)) {
		if (board->selected(ycell_sel, xcell_sel))
		{
			board->unselectCell(ycell_sel, xcell_sel);
			src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		}
		else if (board->BitboardGetColor(ycell_sel, xcell_sel) == board->BitboardGetTurn())
		{
			unselectAll();
			board->selectCell(ycell_sel, xcell_sel);
		}
	}

}
void Game::registerCall(int xcell_sel, int ycell_sel) {
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < N)) {
		if (movement.sourceSquare<0)
		{
			src[0] = xcell_sel, src[1] = ycell_sel;
		}
		else {
			dest[0] = xcell_sel, dest[1] = ycell_sel;
		}
	}
}

void Game::makeMove() {

	bool turn = board->BitboardGetTurn();

	if (!board->scanCheckMate(turn) && turn == bot) {
		movement = machine.botMove(turn);
		movement.pieceType = board->BitboardGetType(movement.sourceSquare);
	}

	else if(!board->scanCheckMate(turn) && turn == player) {
		movement = { board->coordToPos(src[1], src[0]),board->coordToPos(dest[1], dest[0]), board->BitboardGetType(src[1], src[0]) };
	}


	if (movement.sourceSquare >= 0 && movement.targetSquare >= 0) {

		if (board->isLegalmove(movement)) {
			board->makeMove(movement);

			if (!board->scanCheckMate(!turn)) {

				if (board->scanChecks(!turn)) cout << "\ncheck";


			}
			else { cout << "\ncheckamte"; game_over = true; }
		}

		//reset
		src[0] = -1; dest[0] = -1; src[1] = -1; dest[1]=-1;
	}
}
