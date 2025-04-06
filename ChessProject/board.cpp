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

void Board::setBoard() {
	//set initial position
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			tab[i][j].setCell(i, j, Object::EMPTY_CELL, Object::NONE);
		}
	}
	for (int j = 0; j < M; j++) {
		tab[1][j].setCell(1, j, Object::PAWN, Object::BLACK);
	}
	for (int j = 0; j < M; j++) {
		tab[N - 2][j].setCell(N - 2, j, Object::PAWN, Object::WHITE);
	}
	tab[0][0].setCell(0, 0, Object::ROOK, Object::BLACK);
	tab[0][7].setCell(0, 7, Object::ROOK, Object::BLACK);
	tab[7][0].setCell(7, 0, Object::ROOK, Object::WHITE);
	tab[7][7].setCell(7, 7, Object::ROOK, Object::WHITE);

	tab[0][2].setCell(0, 2, Object::BISHOP, Object::BLACK);
	tab[0][5].setCell(0, 5, Object::BISHOP, Object::BLACK);
	tab[7][2].setCell(7, 2, Object::BISHOP, Object::WHITE);
	tab[7][5].setCell(7, 5, Object::BISHOP, Object::WHITE);

	tab[0][1].setCell(0, 1, Object::KNIGHT, Object::BLACK);
	tab[0][6].setCell(0, 6, Object::KNIGHT, Object::BLACK);
	tab[7][1].setCell(7, 1, Object::KNIGHT, Object::WHITE);
	tab[7][6].setCell(7, 6, Object::KNIGHT, Object::WHITE);

	tab[7][3].setCell(7, 3, Object::QUEEN, Object::WHITE);
	tab[0][3].setCell(0, 3, Object::QUEEN, Object::BLACK);

	tab[7][4].setCell(7, 4, Object::KING, Object::WHITE);
	tab[0][4].setCell(0, 4, Object::KING, Object::BLACK);


	listPieces();
	cout << "Tablero inicializado. Piezas: " << board_pieces.size() << endl;
}

//optimizar validez de movimientos
bool Board::validateRayMove(movement move, bool checkDiagonals) {
	int dy = move.destination.y - move.source.y;
	int dx = move.destination.x - move.source.x;

	//condicion de movimiento diagonal
	if(dy == 0 && dx == 0) return false;
	if (checkDiagonals) {
		if (abs(dy) != abs(dx)) return false;
	}
	else {
		if (dy != 0 && dx != 0) return false;
	}
	//determina la direccion del movimiento en x e y
	int stepY = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;
	int stepX = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;

	//establecemos rango de bucle 
	int steps = max(abs(dy), abs(dx));
	for (int i = 1; i < steps; i++) {
		if (getTab()[move.source.y + i * stepY][move.source.x + i * stepX].getColor() != NONE)return false;
	}
	return true;
}
bool Board::pawnMove(movement&pawn) {
	if (pawn.source.color == WHITE)
	{
		if ((pawn.source.y == getSizeY() - 2) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y - 2) && (pawn.destination.color == NONE) && getTab()[pawn.destination.y + 1][pawn.destination.x].getColor() == NONE) {

			//aquí hay que insertar un flag para el en passant
			return true;
		}
		else if ((pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y - 1) && (pawn.destination.color == NONE))
		{
			//movimiento recto
			return true;
		}
		else if ((pawn.source.x + 1 == pawn.destination.x || pawn.source.x - 1 == pawn.destination.x) && pawn.source.y - 1 == pawn.destination.y && (pawn.destination.color == BLACK || getTab()[pawn.destination.y][pawn.destination.x].getFlag() == Object::BLACK_EP))
		{
			//comida diagonal

			return true;
		}
	}
	else if (pawn.source.color == BLACK)
	{
		if ((pawn.source.y == 1) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y + 2) && (pawn.destination.color == NONE) && (getTab()[pawn.destination.y - 1][pawn.destination.x].getColor() == NONE)) {

			//aquí hay que insertar un flag para el en passant
			return true;
		}
		else if (pawn.source.x == pawn.destination.x && pawn.destination.y == pawn.source.y + 1 && pawn.destination.color == NONE)
		{
			//movimiento recto
			return true;
		}
		else if ((pawn.source.x + 1 == pawn.destination.x || pawn.source.x - 1 == pawn.destination.x) && pawn.source.y + 1 == pawn.destination.y && (pawn.destination.color == WHITE || getTab()[pawn.destination.y][pawn.destination.x].getFlag() == Object::WHITE_EP))
		{
			//comida diagonal
			return true;
		}
	}
	return false;
}
bool Board::rookMove(movement& rook) {
	return validateRayMove(rook, false);
}
bool Board::bishopMove(movement& bishop) {
	return validateRayMove(bishop, true);
}
bool Board::knigthMove(movement& knigth) {
	int dy = abs(knigth.destination.y - knigth.source.y);
	int dx = abs(knigth.destination.x - knigth.source.x);
	return (dy == 2 && dx == 1) || (dy == 1 && dx == 2);
}
bool Board::queenMove(movement& queen) {
	return (bishopMove(queen)|| rookMove(queen));
}
bool Board::kingMove(movement& king) {
	if ((abs(king.destination.x - king.source.x) == 1 || abs(king.destination.x - king.source.x) == 0) && (abs(king.destination.y - king.source.y) == 1 || abs(king.destination.y - king.source.y) == 0)) {
		if (abs(king.destination.y - king.source.y) + abs(king.destination.x - king.source.x) != 0)
		{
			return true;
		}
	}
	return false;
}
bool Board::isLegalmove(movement& move) {
	if (
		move.destination.y < 0 || move.destination.y >= getSizeY() ||
		move.destination.x < 0 || move.destination.x >= getSizeX() ||
		move.source.y < 0 || move.source.y >= getSizeY() ||
		move.source.x < 0 || move.source.x >= getSizeX()) {
		return false;
	}
	if ((move.source.color == move.destination.color) && (move.destination.color != NONE) && !isKramnik())return false;
	
	switch (move.source.type)
	{
	case PAWN:return pawnMove(move);
	case ROOK:return rookMove(move);
	case BISHOP:return bishopMove(move);
	case KNIGHT:return knigthMove(move);
	case QUEEN:return (rookMove(move)|| bishopMove(move));
	case KING:return kingMove(move);
	case EMPTY_CELL:return false;
	default:return false;
	}
	return false;
}

void Board::listPieces() {
	board_pieces.clear();
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (getTab()[i][j].getColor() != NONE) {
				board_pieces.push_back({ i,j,getTab()[i][j].getColor(),getTab()[i][j].getType() });
			}
		}
	}
}

//funcion interfaz para modificar las celdas y actualizar la lista de piezas
void Board::updateBoardPiece(int x, int y, Object::type_t type, Object::color_t color) {
	// Eliminar la pieza anterior si existe
	auto it = remove_if(board_pieces.begin(), board_pieces.end(),
		[x, y](const board_piece& p) { return p.x == x && p.y == y; });
	board_pieces.erase(it, board_pieces.end());

	// Añadir nueva pieza si no es vacía
	if (type != Object::EMPTY_CELL || color != Object::NONE) {
		board_pieces.push_back({ y, x, color, type });
	}

	// Actualizar la celda en el tablero
	tab[y][x].setCell(x, y, type, color);
}
