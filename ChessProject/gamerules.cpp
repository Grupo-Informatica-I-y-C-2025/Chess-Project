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
		else if (m_board->getTab()[src[0]][src[1]].getColor() == turn) {
			switch (m_board->getTab()[src[0]][src[1]].getType())
			{
			case Object::PAWN:
				pawnMove();
				break;

			case Object::ROOK:
				rookMove();
				break;

			case Object::BISHOP:
				bishopMove();
				break;

			case Object::KNIGTH:
				knigthMove();
				break;

			case Object::QUEEN:
				queenMove();
				break;

			case Object::KING:
				kingMove();
				break;

			case Object::EMPTY_CELL:

				std::cout << "You do not have a piece there" << std::endl;
				return false;
				break;

			default:

				std::cerr << "Something went wrong in the switch statement in makeMove()" << std::endl;
				return false;
				break;
			}
		}

		//fin reset
		src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		unselectAll();
	}
}

void BoardGL::doMove(Object::type_t t) {
	//mueve la pieza de src a dest, vacía src y desactiva la acción
	m_board->getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1], t, m_board->getTab()[src[0]][src[1]].getColor());
	m_board->getTab()[src[0]][src[1]].setCell(src[0], src[1], Piece::EMPTY_CELL, Piece::NONE);
	action = 0;
}

void BoardGL::changeTurn() {
	if (turn == Object::BLACK)turn = Object::WHITE;
	else turn = Object::BLACK;
}

bool BoardGL::pawnMove() {
	if (m_board->getTab()[src[0]][src[1]].getColor() == Object::WHITE)
	{
		if ((src[1] == dest[1]) && (dest[0] == src[0] - 1) && (m_board->getTab()[dest[0]][dest[1]].getColor() == Object::NONE))
		{
			//movimiento recto
			doMove(Object::PAWN);
			changeTurn();
			return true;
		}
		else if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] - 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::BLACK)
		{
			//comida diagonal
			doMove(Object::PAWN);
			changeTurn();
			return true;
		}
		else return false;
	}
	else if (m_board->getTab()[src[0]][src[1]].getColor() == Object::BLACK)
	{
		if (src[1] == dest[1] && dest[0] == src[0] + 1 && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::NONE)
		{
			//movimiento recto
			doMove(Object::PAWN);
			changeTurn();
			return true;
		}
		else if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] + 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::WHITE)
		{
			//comida diagonal
			doMove(Object::PAWN);
			changeTurn();
			return true;
		}
		else return false;
	}
	else return false;
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
			else//Y+
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
			else//X+
				for (int i = src[1] + 1; i < dest[1]; i++)
					if (m_board->getTab()[dest[0]][i].getColor() != Object::NONE)
						valid = false;
		}
		else  valid = false;
	}
	if (valid)
	{
		doMove(Object::ROOK);
		changeTurn();
		return true;
	}
	else return false;
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
	if (valid)
	{
		doMove(Object::BISHOP);
		changeTurn();
		return true;
	}
	else return false;
}

bool BoardGL::knigthMove() {
	if ((abs(src[1] - dest[1]) == 2 && abs(src[0] - dest[0]) == 1) || (abs(src[1] - dest[1]) == 1 && abs(src[0] - dest[0]) == 2))
	{
		doMove(Object::KNIGTH);
		changeTurn();
		return true;
	}
	else
	{
		return false;
	}
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
			else//Y+
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
			else//X+
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
	if (valid)
	{
		doMove(Object::QUEEN);
		changeTurn();
		return true;
	}
	else return false;
}

bool BoardGL::kingMove() {
	if ((abs(dest[1] - src[1]) == 1 || abs(dest[1] - src[1]) == 0)&&(abs(dest[0] - src[0]) == 1 || abs(dest[0] - src[0]) == 0))
		if (abs(dest[0] - src[0]) + abs(dest[1] - src[1]) != 0)
		{
			doMove(Object::KING);
			changeTurn();
			return true;
		}
		else return false;
	else return false;
}