#include "freeglut.h"
#include "boardGL.h"
#include <iostream>
#include <math.h>

void BoardGL::unselectAll() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			m_board->getTab()[i][j].unselectCell();
			action = 0;
		}
	}
}

void BoardGL::activate()
{
	////Activar casillas -- parte propia	
	if (m_board->getTab()[xcell_sel][ycell_sel].selected())
	{
		m_board->getTab()[xcell_sel][ycell_sel].unselectCell();
		action = 0;
	}
	else if (m_board->getTab()[xcell_sel][ycell_sel].getType() != Object::EMPTY_CELL)
	{
		unselectAll();
		m_board->getTab()[xcell_sel][ycell_sel].selectCell();
		action = 1;
	}

}

void BoardGL::registerCall() {
	if (!action)
	{
		src[0] = xcell_sel, src[1] = ycell_sel;
	}
	else {
		dest[0] = xcell_sel, dest[1] = ycell_sel;
	}
}

bool BoardGL::makeMove() {
	if (src[0] != -1 && src[1] != -1 && dest[0] != -1 && dest[1] != -1) {

		
		if ((m_board->getTab()[src[0]][src[1]].getColor() == m_board->getTab()[dest[0]][dest[1]].getColor()) && (m_board->getTab()[dest[0]][dest[1]].getColor() != Object::NONE))
		{
			return false;
		}
		else {
			//cout << turn;
			
			if (m_board->getTab()[src[0]][src[1]].getColor() == turn ) {
				/*if (turn == Object::WHITE) {
					if (!scanChecks(turn, wkx, wky)) {
						cout << "\n w check!";
					}
				}
				else if (turn == Object::BLACK) {
					if (!scanChecks(turn, bkx, bky)) {
						cout << "\n b check!";
					}
				}*/

				switch (m_board->getTab()[src[0]][src[1]].getType())
				{
				case Object::PAWN:
					if(pawnMove())doMove();
					return true;
					break;

				case Object::ROOK:
					if (rookMove())doMove();
					return true;
					break;

				case Object::BISHOP:
					if(bishopMove())doMove();
					return true;
					break;

				case Object::KNIGTH:
					if(knigthMove())doMove();
					return true;
					break;

				case Object::QUEEN:
					if(queenMove())doMove();
					return true;
					break;

				case Object::KING:

					if(kingMove())doMove();
					return true;
					break;

				case Object::EMPTY_CELL:

					std::cout << "No deberías poder mover una casilla vacía " << std::endl;
					return false;
					break;

				default:

					std::cerr << "Ha ocurrido un error" << std::endl;
					return false;
					break;
				}
			}
		}

		//fin reset
		unselectAll();
	}
}

void BoardGL::doMove() {
	//mueve la pieza de src a dest, vacía src y desactiva la acción
	//findKing(turn);

	//realiza el movimiento de forma virtual, si el movimiento resulta dejar al propio rey en jaque cancela el movimiento

	//almacenamos los datos de la casilla de origen:

	Object::type_t source_t = m_board->getTab()[src[0]][src[1]].getType(), destination_t = m_board->getTab()[dest[0]][dest[1]].getType();
	Object::color_t source_c = m_board->getTab()[src[0]][src[1]].getColor(), destination_c = m_board->getTab()[dest[0]][dest[1]].getColor();
	m_board->getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1],source_t,source_c);
	m_board->getTab()[src[0]][src[1]].setCell(src[0], src[1], Piece::EMPTY_CELL, Piece::NONE);

	findKing(turn);
	if (scanChecks(turn, kx, ky)) {
		action = 0;
		src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		changeTurn();
		if (turn == Object::WHITE)cout << "\nwhites turn";
		else cout << "\nblacks turn";
	}
	else {
		//cout << "\n invalid move because result in check!";
		m_board->getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1], destination_t, destination_c);
		m_board->getTab()[src[0]][src[1]].setCell(src[0], src[1], source_t, source_c);
	}

	
	
}

void BoardGL::changeTurn() {
	if (turn == Object::BLACK)turn = Object::WHITE;
	else turn = Object::BLACK;
}

bool BoardGL::pawnMove() {
	bool valid = false;
	if (m_board->getTab()[src[0]][src[1]].getColor() == Object::WHITE)
	{
		if ((src[1] == dest[1]) && (dest[0] == src[0] - 1) && (m_board->getTab()[dest[0]][dest[1]].getColor() == Object::NONE))
		{
			//movimiento recto
			valid = true;
		}
		else if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] - 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::BLACK)
		{
			//comida diagonal
			valid = true;
		}
	}
	else if (m_board->getTab()[src[0]][src[1]].getColor() == Object::BLACK)
	{
		if (src[1] == dest[1] && dest[0] == src[0] + 1 && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::NONE)
		{
			//movimiento recto
			valid = true;
		}
		else if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] + 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::WHITE)
		{
			//comida diagonal
			valid = true;
		}
	}
	return valid;
}

bool BoardGL::rookMove() {
	bool valid = true;
	if (src[1] != dest[1] || src[0] != dest[0])//movimientos lineales
	{

		if (src[1] == dest[1])// movimiento en X
		{
			int yrange = dest[0] - src[0];//Y
			if (yrange < 0)//Y-
				for (int i = src[0] - 1; i > dest[0]; i--)
					if (m_board->getTab()[i][dest[1]].getColor() != Object::NONE)
						valid = false;
			if (yrange > 0)//Y+
				for (int i = src[0] + 1; i < dest[0]; i++)
					if (m_board->getTab()[i][dest[1]].getColor() != Object::NONE)
						valid = false;
		}
		else if (src[0] == dest[0])//movimiento en Y
		{
			int xrange = dest[1] - src[1];//X-
			if (xrange < 0)
				for (int i = src[1] - 1; i > dest[1]; i--)
					if (m_board->getTab()[dest[0]][i].getColor() != Object::NONE)
						valid = false;
			if (xrange > 0)//X+
				for (int i = src[1] + 1; i < dest[1]; i++)
					if (m_board->getTab()[dest[0]][i].getColor() != Object::NONE)
						valid = false;
		}
		else  valid = false;
	}
	return valid;
}

bool BoardGL::bishopMove() {
	bool valid = true;
	if (abs(src[1] - dest[1]) == abs(src[0] - dest[0]))//movimientos diagonales
	{
		int xIncrement = (dest[1] - src[1]) / (abs(dest[1] - src[1]));
		int yIncrement = (dest[0] - src[0]) / (abs(dest[0] - src[0]));
		for (int i = 1; i < abs(src[1] - dest[1]); i++)
		{
			if (m_board->getTab()[src[0] + yIncrement * i][src[1] + xIncrement * i].getColor() != Object::NONE)
				valid = false;
		}
	}
	else valid = false;
	return valid;
}

bool BoardGL::knigthMove() {
	bool valid ;
	if ((abs(src[1] - dest[1]) == 2 && abs(src[0] - dest[0]) == 1) || (abs(src[1] - dest[1]) == 1 && abs(src[0] - dest[0]) == 2))
	{
		valid = true;
	}
	else
	{
		valid = false;
	}
	return valid;
}

bool BoardGL::queenMove() {
	bool valid = true;
	if (src[1] != dest[1] || src[0] != dest[0])//movimientos lineales
	{
		if (src[1] == dest[1])// movimiento en X
		{
			int yrange = dest[0] - src[0];//Y
			if (yrange < 0)//Y-
				for (int i = src[0] - 1; i > dest[0]; i--)
					if (m_board->getTab()[i][dest[1]].getColor() != Object::NONE)
						valid = false;
			if (yrange > 0)//Y+
				for (int i = src[0] + 1; i < dest[0]; i++)
					if (m_board->getTab()[i][dest[1]].getColor() != Object::NONE)
						valid = false;
		}
		else if (src[0] == dest[0])//movimiento en Y
		{
			int xrange = dest[1] - src[1];//X-
			if (xrange < 0)
				for (int i = src[1] - 1; i > dest[1]; i--)
					if (m_board->getTab()[dest[0]][i].getColor() != Object::NONE)
						valid = false;
			if (xrange > 0)//X+
				for (int i = src[1] + 1; i < dest[1]; i++)
					if (m_board->getTab()[dest[0]][i].getColor() != Object::NONE)
						valid = false;
		}
		else if (abs(src[1] - dest[1]) == abs(src[0] - dest[0]))//movimientos diagonales
		{
			int xIncrement = (dest[1] - src[1]) / (abs(dest[1] - src[1]));
			int yIncrement = (dest[0] - src[0]) / (abs(dest[0] - src[0]));
			for (int i = 1; i < abs(src[1] - dest[1]); i++)
			{
				if (m_board->getTab()[src[0] + yIncrement * i][src[1] + xIncrement * i].getColor() != Object::NONE)
					valid = false;
			}
		}
		else valid = false;
	}
	return valid;
}

bool BoardGL::kingMove() {
	bool valid = false;
	if ((abs(dest[1] - src[1]) == 1 || abs(dest[1] - src[1]) == 0)&&(abs(dest[0] - src[0]) == 1 || abs(dest[0] - src[0]) == 0))
		if (abs(dest[0] - src[0]) + abs(dest[1] - src[1]) != 0)
		{
			valid = true;
		}
	return valid;
}

bool BoardGL::scanChecks(Object::color_t c,int king_x,int king_y) {
	bool check = false;
	//check lineales
	
	
	//Y-
	for (int i = king_y -1; i > 0; i--) {
		if (m_board->getTab()[i][king_x].getColor() != c && m_board->getTab()[i][king_x].getColor() != Object::NONE ) {
			if (m_board->getTab()[i][king_x].getType() == Object::ROOK || m_board->getTab()[i][king_x].getType() == Object::QUEEN) {
				check = true;
				//cout << "\nfound attacking piece in y-";
				break;
			}
		}
		else if(m_board->getTab()[i][king_x].getColor() == c)break;
		
	}
	//Y+
	for (int i = king_y+1; i < N; i++) {
		if (m_board->getTab()[i][king_x].getColor() != c && m_board->getTab()[i][king_x].getColor() != Object::NONE) {
			if (m_board->getTab()[i][king_x].getType() == Object::ROOK || m_board->getTab()[i][king_x].getType() == Object::QUEEN) {
				check = true; 
				//cout << "\nfound attacking piece in y+";
			}
		}
		else if (m_board->getTab()[i][king_x].getColor() == c)break;
	}


	//X-
	for (int i = king_x -1; i > 0; i--) {
		if (m_board->getTab()[king_y][i].getColor() != c && m_board->getTab()[king_y][i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y][i].getType() == Object::ROOK || m_board->getTab()[king_y][i].getType() == Object::QUEEN) {
				check = true;
				//cout << "\nfound attacking piece in x-";
			}
		}
		else if (m_board->getTab()[king_y][i].getColor() == c)break;
	}		

	//X+
	for (int i = king_x +1; i < N; i++) {
		if (m_board->getTab()[king_y][i].getColor() != c && m_board->getTab()[king_y][i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y][i].getType() == Object::ROOK || m_board->getTab()[king_y][i].getType() == Object::QUEEN) {
				check = true;
				//cout << "\nfound attacking piece in x+";
			}
		}
		else if (m_board->getTab()[king_y][i].getColor() == c)break;
	}

	//movimientos diagonales
	/*int xIncrement = 1;
	int yIncrement = 1;

	if(!(king_x == N-1 ||king_y == N-1))for (int i = 1; i < N ; i++)
	{
		if (king_y + yIncrement * i < 0 || king_y + yIncrement * i >N)break;
		if (king_x + xIncrement * i < 0 || king_x + xIncrement * i >N)break;
		//cout<< "x, y " << king_y + yIncrement * i << " " << king_x + xIncrement * i;
		if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != c && m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::BISHOP || m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::QUEEN) { 
				check = false; 
				cout << "\nfound attacking piece in x+ y+";
			}

		}
		else if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() == c)break;
	}



	xIncrement = 1;
	yIncrement = -1;
	if (!(king_x == N-1 || king_y == 0))for (int i = 1; i <  N ; i++)
	{
		if (king_y + yIncrement * i < 0 || king_y + yIncrement * i >N)break;
		if (king_x + xIncrement * i < 0 || king_x + xIncrement * i >N)break;
		cout << "\n" << king_y + yIncrement * i;
		cout << " : " << king_x + xIncrement * i;
		if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != c && m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::BISHOP || m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::QUEEN) {
				check = false;
				cout << "\nfound attacking piece in x+ y-";
			}
		}
		else if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() == c)break;
	}
	xIncrement = -1;
	yIncrement = 1;
	if (!(king_x == 0 || king_y == N-1))for (int i = 1; i < N ; i++)
	{
		if (king_y + yIncrement * i < 0 || king_y + yIncrement * i >N)break;
		if (king_x + xIncrement * i < 0 || king_x + xIncrement * i >N)break;
		if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != c && m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::BISHOP || m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::QUEEN) {
				check = false;
				cout << "\nfound attacking piece in x- y+";
			}
		}
		else if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() == c)break;
	}
	xIncrement = -1;
	yIncrement = -1;
	if (!(king_x == 0 || king_y == 0))for (int i = 1; i < N; i++)
	{
		if (king_y + yIncrement * i < 0 || king_y + yIncrement * i >N)break;
		if (king_x + xIncrement * i < 0 || king_x + xIncrement * i >N)break;
		if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != c && m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() != Object::NONE) {
			if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::BISHOP || m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getType() == Object::QUEEN) {
				check = false;
				cout << "\nfound attacking piece in x- y-";
			}
		}
		else if (m_board->getTab()[king_y + yIncrement * i][king_x + xIncrement * i].getColor() == c)break;
	}*/


	return !check;
}

void BoardGL::findKing(Object::color_t c) {
	bool found = false;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (m_board->getTab()[i][j].getColor() == c && m_board->getTab()[i][j].getType() == Object::KING) {
				found = true;
				kx = j; ky = i;
				//cout << "\nx y: " << kx << ky;
				break;
			}
		}
		if (found) { break; }
	}
}

bool BoardGL::scanCheckMate(Object::color_t c) {
	if (true) {
		return true;
	}
	else return false;
}
