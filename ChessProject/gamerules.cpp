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


void Board::unselectAll() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			getTab()[i][j].unselectCell();
			action = 0;
		}
	}
}

void Board::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel>=0 && xcell_sel<N) && (ycell_sel >= 0 && ycell_sel < N)) {
		if (getTab()[xcell_sel][ycell_sel].selected())
		{
			getTab()[xcell_sel][ycell_sel].unselectCell();
			action = 0;
			src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		}
		else if (getTab()[xcell_sel][ycell_sel].getColor() == turn)
		{
			unselectAll();
			getTab()[xcell_sel][ycell_sel].selectCell();
			action = 1;
		}
	}

}

void Board::registerCall(int xcell_sel, int ycell_sel) {
	
	if (!action)
	{
		src[0] = xcell_sel, src[1] = ycell_sel;
	}
	else {
		dest[0] = xcell_sel, dest[1] = ycell_sel;
	}
}

bool Board::makeMove() {
	if (scanCheckMate(turn))cout << "\n checkmate";
	else {
		if (src[0] != -1 && src[1] != -1 && dest[0] != -1 && dest[1] != -1 ) {

			if (getTab()[src[0]][src[1]].getColor() == turn) {

				switch (getTab()[src[0]][src[1]].getType())
				{
				case PAWN:

					//cout << "\nfound pawn";
					if (pawnMove(src[0], src[1], dest[0], dest[1])) {
						enPassant(dest[0], dest[1]);
						doMove();
					}
					return true;
					break;

				case ROOK:

					//cout << "\nfound rook";
					if (rookMove(src[0], src[1], dest[0], dest[1]))doMove();
					return true;
					break;

				case BISHOP:

					//cout << "\nfound bishop";
					if (bishopMove(src[0], src[1], dest[0], dest[1]))doMove();
					return true;
					break;

				case KNIGHT:

					//cout << "\nfound knigth";
					if (knigthMove(src[0], src[1], dest[0], dest[1]))doMove();
					return true;
					break;

				case QUEEN:

					//cout << "\nfound queen";
					if (queenMove(src[0], src[1], dest[0], dest[1]))doMove();
					return true;
					break;

				case KING:

					//cout << "\nfound king";
					if (kingMove(src[0], src[1], dest[0], dest[1]))doMove();
					return true;
					break;


				case EMPTY_CELL:

					std::cout << "No deberías poder mover una casilla vacía (ver funcion activate) " << std::endl;
					return false;
					break;

				default:

					std::cerr << "Ha ocurrido un error" << std::endl;
					return false;
					break;
				}
			}
		}
	}
}

void Board::doMove() {
	//mueve la pieza de src a dest, vacía src y desactiva la acción
	//findKing(turn);

	//realiza el movimiento de forma virtual, si el movimiento resulta dejar al propio rey en jaque cancela el movimiento

	//almacenamos los datos de la casilla de origen:

	Object::type_t source_t = getTab()[src[0]][src[1]].getType(), destination_t = getTab()[dest[0]][dest[1]].getType();
	Object::color_t source_c = getTab()[src[0]][src[1]].getColor(), destination_c = getTab()[dest[0]][dest[1]].getColor();
	getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1],source_t,source_c);
	getTab()[src[0]][src[1]].setCell(src[0], src[1], Piece::EMPTY_CELL, Piece::NONE);

	if (!scanChecks(turn) && findKing(turn)) {
		//cout<<"\nscore : "<<EvaluateGame();
		changeTurn();
		if (turn == bot) {
			int alpha = -10000, beta = 10000;
			cout << "\nminmax :" << minimax(4,&alpha,&beta, true);
		}

	}
	else {
		getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1], destination_t, destination_c);
		getTab()[src[0]][src[1]].setCell(src[0], src[1], source_t, source_c);
	}
}

void Board::changeTurn() {//referencia a cambiar de turno o traer turno <aqui

	if (turn == BLACK)turn = WHITE;
	else turn = BLACK;

	if (!scanCheckMate(turn)) {
		if (scanChecks(turn)) {
			cout << "\n check!";
		}
		if (turn == WHITE)cout << "\nwhites turn";
		else cout << "\nblacks turn";
	}

	

	//reset
	cleanEnpassant(turn);
	action = 0;
	unselectAll();
	src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
}

bool Board::pawnMove(int sy,int sx, int dy,int dx) {
	bool valid = false;
	if ((getTab()[sy][sx].getColor() == getTab()[dy][dx].getColor()) && (getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else {
		if (getTab()[sy][sx].getColor() == WHITE)
		{
			if ((sy == 6) && (sx == dx) && (dy == sy - 2) && (getTab()[dy][dx].getColor() == NONE) && getTab()[dy+1][dx].getColor() == NONE) {
				
				//aquí hay que insertar un flag para el en passant
				getTab()[dy+1][dx].setFlag(dy+1, dx, Object::WHITE_EP);
				valid = true;
			}
			else if ((sx == dx) && (dy == sy - 1) && (getTab()[dy][dx].getColor() == NONE))
			{
				//movimiento recto
				valid = true;
			}
			else if ((sx + 1 == dx || sx - 1 == dx) && sy - 1 == dy && (getTab()[dy][dx].getColor() == BLACK || getTab()[dy][dx].getFlag() == Object::BLACK_EP))
			{
				//comida diagonal
				
				valid = true;
			}
		}
		else if (getTab()[sy][sx].getColor() == BLACK)
		{
			if ((sy == 1) && (sx == dx) && (dy == sy + 2) && (getTab()[dy][dx].getColor() == NONE) && (getTab()[dy -1][dx].getColor() == NONE)) {
				
				//aquí hay que insertar un flag para el en passant
				getTab()[dy - 1][dx].setFlag(dy - 1, dx, Object::BLACK_EP);
				valid = true;
			}
			else if (sx == dx && dy == sy + 1 && getTab()[dy][dx].getColor() == NONE)
			{
				//movimiento recto
				valid = true;
			}
			else if ((sx + 1 == dx || sx - 1 == dx) && sy + 1 == dy && (getTab()[dy][dx].getColor() == WHITE || getTab()[dy][dx].getFlag() == Object::WHITE_EP))
			{
				//comida diagonal
				valid = true;
			}
		}
	}
	return valid;
}

bool Board::rookMove(int sy, int sx, int dy, int dx) {
	bool valid = true;
	if (( getTab()[sy][sx].getColor() ==  getTab()[dy][dx].getColor()) && ( getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else
	if (sx != dx || sy != dy)//movimientos lineales
	{

		if (sx == dx)// movimiento en X
		{
			int yrange = dy - sy;//Y
			if (yrange < 0)//Y-
				for (int i = sy - 1; i > dy; i--)
					if ( getTab()[i][dx].getColor() != NONE)
						valid = false;
			if (yrange > 0)//Y+
				for (int i = sy + 1; i < dy; i++)
					if ( getTab()[i][dx].getColor() != NONE)
						valid = false;
		}
		else if (sy == dy)//movimiento en Y
		{
			int xrange = dx - sx;//X-
			if (xrange < 0)
				for (int i = sx - 1; i > dx; i--)
					if ( getTab()[dy][i].getColor() != NONE)
						valid = false;
			if (xrange > 0)//X+
				for (int i = sx + 1; i < dx; i++)
					if ( getTab()[dy][i].getColor() != NONE)
						valid = false;
		}
		else  valid = false;
	}
	return valid;
}

bool Board::bishopMove(int sy, int sx, int dy, int dx) {
	bool valid = true;
	if (( getTab()[sy][sx].getColor() ==  getTab()[dy][dx].getColor()) && ( getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else {
		if (abs(sx - dx) == abs(sy - dy))//movimientos diagonales
		{
			int xIncrement = (dx - sx) / (abs(dx - sx));
			int yIncrement = (dy - sy) / (abs(dy - sy));
			for (int i = 1; i < abs(sx - dx); i++)
			{
				if ( getTab()[sy + yIncrement * i][sx + xIncrement * i].getColor() != NONE)
					valid = false;
			}
		}
		else valid = false;
	}
	return valid;
}

bool Board::knigthMove(int sy, int sx, int dy, int dx) {
	bool valid ;
	if (( getTab()[sy][sx].getColor() ==  getTab()[dy][dx].getColor()) && ( getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else {
		if ((abs(sx - dx) == 2 && abs(sy - dy) == 1) || (abs(sx - dx) == 1 && abs(sy - dy) == 2))
		{
			valid = true;
		}
		else
		{
			valid = false;
		}
	}
	return valid;
}

bool Board::queenMove(int sy, int sx, int dy, int dx) {
	bool valid = true;
	if (( getTab()[sy][sx].getColor() ==  getTab()[dy][dx].getColor()) && ( getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else
	if (sx != dx || sy != dy)//movimientos lineales
	{
		if (sx == dx)// movimiento en X
		{
			int yrange = dy - sy;//Y
			if (yrange < 0)//Y-
				for (int i = sy - 1; i > dy; i--)
					if ( getTab()[i][dx].getColor() != NONE)
						valid = false;
			if (yrange > 0)//Y+
				for (int i = sy + 1; i < dy; i++)
					if ( getTab()[i][dx].getColor() != NONE)
						valid = false;
		}
		else if (sy == dy)//movimiento en Y
		{
			int xrange = dx - sx;//X-
			if (xrange < 0)
				for (int i = sx - 1; i > dx; i--)
					if ( getTab()[dy][i].getColor() != NONE)
						valid = false;
			if (xrange > 0)//X+
				for (int i = sx + 1; i < dx; i++)
					if ( getTab()[dy][i].getColor() != NONE)
						valid = false;
		}
		else if (abs(sx - dx) == abs(sy - dy))//movimientos diagonales
		{
			int xIncrement = (dx - sx) / (abs(dx - sx));
			int yIncrement = (dy - sy) / (abs(dy - sy));
			for (int i = 1; i < abs(sx - dx); i++)
			{
				if ( getTab()[sy + yIncrement * i][sx + xIncrement * i].getColor() !=NONE)
					valid = false;
			}
		}
		else valid = false;
	}
	return valid;
}

bool Board::kingMove(int sy, int sx, int dy, int dx) {
	bool valid = false;
	if (( getTab()[sy][sx].getColor() ==  getTab()[dy][dx].getColor()) && ( getTab()[dy][dx].getColor() != NONE) && !isKramnik())
	{
		valid = false;
	}
	else {
		if ((abs(dx - sx) == 1 || abs(dx - sx) == 0) && (abs(dy - sy) == 1 || abs(dy - sy) == 0)) {
			if (abs(dy - sy) + abs(dx - sx) != 0)
			{
				valid = true;
			}
		}
		else valid = false;
	}
	return valid;
}

//parece que en los movimientos lineales y diagonales solo impide tener jaque piezas de su mismo color
//**implementado correctamente la modificación del algoritmo

bool Board::scanChecks(Object::color_t c) {
	bool check = false;

	findKing(c);
	//movimientos lineales lineales
	for (int i = ky -1; i > 0; i--) {//Y-
		if ( getTab()[i][kx].getColor() != c &&  getTab()[i][kx].getColor() != NONE && (getTab()[i][kx].getType() == ROOK || getTab()[i][kx].getType() == QUEEN)) 
		{
			check = true;
			//cout << "\nfound attacking piece in y-";
			break;
		}
		
		else if( getTab()[i][kx].getColor() != NONE)break;
		
	}
	for (int i = ky+1; i < N; i++) {//Y+
		if ( getTab()[i][kx].getColor() != c &&  getTab()[i][kx].getColor() != NONE && (getTab()[i][kx].getType() == ROOK || getTab()[i][kx].getType() == QUEEN)) 
		{
			check = true;
			//cout << "\nfound attacking piece in y+";
			break;
		}
		
		else if ( getTab()[i][kx].getColor() != NONE)break;
	}
	for (int i = kx -1; i > 0; i--) {//X-
		if ( getTab()[ky][i].getColor() != c &&  getTab()[ky][i].getColor() != NONE && (getTab()[i][kx].getType() == ROOK || getTab()[i][kx].getType() == QUEEN)) 
		{
			check = true;
			//cout << "\nfound attacking piece in x-";
			break;
		}
		else if ( getTab()[ky][i].getColor() != NONE)break;
	}		
	for (int i = kx +1; i < N; i++) {//X+
		if ( getTab()[ky][i].getColor() != c &&  getTab()[ky][i].getColor() != NONE && (getTab()[i][kx].getType() == ROOK || getTab()[i][kx].getType() == QUEEN)) 
		{
			check = true;
			//cout << "\nfound attacking piece in x+";
			break;
		}
		else if ( getTab()[ky][i].getColor() != NONE)break;
	}

	//movimientos diagonales
	int xIncrement = 1;
	int yIncrement = 1;
	for (int i = 1; i < N ; i++)
	{
		if (ky + yIncrement * i < 0 || ky + yIncrement * i >N-1)break;
		if (kx + xIncrement * i < 0 || kx + xIncrement * i >N-1)break;
		//cout<< "x, y " << ky + yIncrement * i << " " << kx + xIncrement * i;
		if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != c &&  getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE && (getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == BISHOP || getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == QUEEN)) 
		{
			check = true; 
			break;
		}
		else if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE)break;
	}
	xIncrement = 1;
	yIncrement = -1;
	for (int i = 1; i <  N ; i++)
	{
		if (ky + yIncrement * i < 0 || ky + yIncrement * i >N-1)break;
		if (kx + xIncrement * i < 0 || kx + xIncrement * i >N-1)break;
		if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != c &&  getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE && (getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == BISHOP || getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == QUEEN))
		{
			check = true;
			break;
		}
		else if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE)break;
	}
	xIncrement = -1;
	yIncrement = 1;
	for (int i = 1; i < N ; i++)
	{
		if (ky + yIncrement * i < 0 || ky + yIncrement * i >N-1)break;
		if (kx + xIncrement * i < 0 || kx + xIncrement * i >N-1)break;
		if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != c &&  getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE && (getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == BISHOP || getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == QUEEN))
		{
			check = true;
			break;
		}
		else if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE)break;
	}
	xIncrement = -1;
	yIncrement = -1;
	for (int i = 1; i < N; i++)
	{
		if (ky + yIncrement * i < 0 || ky + yIncrement * i >N-1)break;
		if (kx + xIncrement * i < 0 || kx + xIncrement * i >N-1)break;
		if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != c &&  getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE && (getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == BISHOP || getTab()[ky + yIncrement * i][kx + xIncrement * i].getType() == QUEEN))
		{
			check = true;
			break;
		}
		else if ( getTab()[ky + yIncrement * i][kx + xIncrement * i].getColor() != NONE)break;
	}

	//movimientos de caballos
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if( getTab()[i][j].getColor() != c &&  getTab()[i][j].getColor() != NONE &&   getTab()[i][j].getType() == KNIGHT)
			if ((abs(j - kx) == 2 && abs(i - ky) == 1) || (abs(j - kx) == 1 && abs(i - ky) == 2))
			{
				check = true;
			}
		}
		if (check) { break; }
	}
	
	//movimientos de peones
	if (c == WHITE) {
		if(ky - 1 >= 0 && ky - 1 < N && kx - 1 >= 0 && kx - 1 < N)
			if ( getTab()[ky-1][kx-1].getType() == PAWN &&  getTab()[ky - 1][kx - 1].getColor() == BLACK)
				check =true;
		if (ky - 1 >= 0 && ky - 1 < N && kx + 1 >= 0 && kx + 1 < N)
			if ( getTab()[ky - 1][kx + 1].getType() == PAWN &&  getTab()[ky - 1][kx + 1].getColor() == BLACK)
				check = true;
	}
	if (c == BLACK) {
		if (ky + 1 >= 0 && ky + 1 < N && kx - 1 >= 0 && kx - 1 < N)
			if ( getTab()[ky + 1][kx - 1].getType() ==  PAWN &&  getTab()[ky + 1][kx - 1].getColor() ==  	WHITE)
				check = true;
		if (ky + 1 >= 0 && ky + 1 < N && kx + 1 >= 0 && kx + 1 < N)
			if ( getTab()[ky + 1][kx + 1].getType() ==  PAWN &&  getTab()[ky + 1][kx + 1].getColor() ==  WHITE)
				check = true;
	}

	return check;
}

bool Board::findKing(Object::color_t c) {
	bool found = false;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if ( getTab()[i][j].getColor() == c &&  getTab()[i][j].getType() ==  KING) {
				found = true;
				kx = j; ky = i;
				break;
			}
		}
		if (found) { break; }
	}
	return found;
}
  
bool Board::scanCheckMate(Object::color_t turn) {
	bool checkmate = true;
	for (int i = 0; i < N; i++) {//busca todas las piezas del color del turno en el tablero
		
		for (int j = 0; j < N; j++) {
			
			if ( getTab()[i][j].getColor() == turn) {

				//escanea todos los movimientos de esa pieza
				//para ver si de entre los movimientos validos alguno impide que se produzca un jaque en el siguiente turno
				//si encuentra una solución se rompe el bucle para no calcular el resto
				switch ( getTab()[i][j].getType())
				{
				case  PAWN:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && pawnMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								bool b_ep = false;
								bool w_ep = false;
								Object::type_t w_ep_t, b_ep_t;
								Object::color_t w_ep_c, b_ep_c;
								//añadir en passant
								auto destination_t =  getTab()[k][l].getType();
								auto destination_c =  getTab()[k][l].getColor();
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
									getTab()[k-1][l].setCell(k-1, l, EMPTY_CELL, NONE);
								}
								if (getTab()[k][l].getFlag() == Object::BLACK_EP) {
									b_ep = true;
									getTab()[k + 1][l].setCell(k + 1, l, EMPTY_CELL, NONE);
								}

								 getTab()[k][l].setCell(k, l,  PAWN, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in :"<< i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i , j ,  PAWN, turn);//src

								if (w_ep) {
									 getTab()[k - 1][l].setCell(k - 1, l, w_ep_t, w_ep_c);
								 }
								 if (b_ep) {
									 getTab()[k + 1][l].setCell(k + 1, l, b_ep_t, b_ep_c);
								 }

								 //clear generated en passant flags
								 if (abs(i-k)==2 && turn == WHITE)getTab()[k+1][l].setFlag(k+1, l, Object::NO_FLAG);
								 if (abs(i - k) == 2 && turn == BLACK)getTab()[k-1][l].setFlag(k - 1, l, Object::NO_FLAG);	
							}
						}
					}
					break;

				case  ROOK:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && rookMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								
								auto destination_t =  getTab()[k][l].getType();
								Object::color_t destination_c =  getTab()[k][l].getColor();
								 getTab()[k][l].setCell(k, l,  ROOK, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in rook:" << i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i, j,  ROOK, turn);//src
							}
						}
					}
					break;

				case  BISHOP:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && bishopMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								
								auto destination_t =  getTab()[k][l].getType();
								Object::color_t destination_c =  getTab()[k][l].getColor();
								 getTab()[k][l].setCell(k, l,  BISHOP, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in bishop:" << i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i, j,  BISHOP, turn);//src
							}
						}
					}
					break;

				case  KNIGHT:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && knigthMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								
								auto destination_t =  getTab()[k][l].getType();
								Object::color_t destination_c =  getTab()[k][l].getColor();
								 getTab()[k][l].setCell(k, l,  KNIGHT, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in knigth:" << i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i, j,  KNIGHT, turn);//src
							}
						}
					}
					break;

				case  QUEEN:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && queenMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								
								auto destination_t =  getTab()[k][l].getType();
								Object::color_t destination_c =  getTab()[k][l].getColor();
								 getTab()[k][l].setCell(k, l,  QUEEN, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in queen:" << i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i, j,  QUEEN, turn);//src
							}
						}
					}
					break;

				case  KING:

					for (int k = 0; k < N; k++) {
						for (int l = 0; l < N; l++) {
							if ( getTab()[k][l].getColor() != turn && kingMove(i, j, k, l)) {//no propia pieza y movimiento valido de peon
								
								auto destination_t =  getTab()[k][l].getType();
								Object::color_t destination_c =  getTab()[k][l].getColor();
								 getTab()[k][l].setCell(k, l,  KING, turn);//dest
								 getTab()[i][j].setCell(i, j,  EMPTY_CELL,  NONE);//src

								//check if result in checkmate 
								if (!scanChecks(turn) && findKing(turn)) {
									//cout << "\n not checkmate in king:" << i << " . " << j << "  to  " << k << " . " << l;
									checkmate = false;
								}
								//undo
								 getTab()[k][l].setCell(k, l, destination_t, destination_c);//dest
								 getTab()[i][j].setCell(i, j,  KING, turn);//src
							}
						}
					}
					break;

				case  EMPTY_CELL:

					std::cout << "Si la casilla esta vacía no debería llegar aqui" << std::endl;
					return false;
					break;

				default:

					std::cerr << "Ha ocurrido un error" << std::endl;
					return false;
					break;
				}

			}
		}
	}
	//if (checkmate)cout << "\n checkmate";
	return checkmate;
}

void Board::enPassant(int dy, int dx) {
	if ( getTab()[dy][dx].getFlag() == Object::WHITE_EP) getTab()[dy - 1][dx].setCell(dy - 1, dx,  EMPTY_CELL,  NONE);
	if ( getTab()[dy][dx].getFlag() == Object::BLACK_EP) getTab()[dy + 1][dx].setCell(dy + 1, dx,  EMPTY_CELL,  NONE);
	
}

void Board::cleanEnpassant(Object::color_t c) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if(c== WHITE)
			if ( getTab()[i][j].getFlag() == Object::WHITE_EP) {
				 getTab()[i][j].setFlag(i, j, Object::NO_FLAG);
				break;
			}
			if (c ==  BLACK)
				if ( getTab()[i][j].getFlag() == Object::BLACK_EP) {
					 getTab()[i][j].setFlag(i, j, Object::NO_FLAG);
					break;
				}
		}
	}
}

void Board::setBoard() {
	//set initial position
	/*for (int j = 0; j < N; j++) {
		tab[1][j].setCell(1, j, Piece::PAWN, Piece::BLACK);
	}
	for (int j = 0; j < N; j++) {
		tab[N - 2][j].setCell(N - 2, j, Piece::PAWN, Piece::WHITE);
	}*/
	tab[0][0].setCell(0, 0, Piece::ROOK, Piece::BLACK);
	tab[0][7].setCell(0, 7, Piece::ROOK, Piece::BLACK);
	tab[7][0].setCell(7, 0, Piece::ROOK, Piece::WHITE);
	tab[7][7].setCell(7, 7, Piece::ROOK, Piece::WHITE);

	tab[0][2].setCell(0, 2, Piece::BISHOP, Piece::BLACK);
	tab[0][5].setCell(0, 5, Piece::BISHOP, Piece::BLACK);
	tab[7][2].setCell(7, 2, Piece::BISHOP, Piece::WHITE);
	tab[7][5].setCell(7, 5, Piece::BISHOP, Piece::WHITE);

	tab[0][1].setCell(0, 1, Piece::KNIGHT, Piece::BLACK);
	tab[0][6].setCell(0, 6, Piece::KNIGHT, Piece::BLACK);
	tab[7][1].setCell(7, 1, Piece::KNIGHT, Piece::WHITE);
	tab[7][6].setCell(7, 6, Piece::KNIGHT, Piece::WHITE);

	tab[7][3].setCell(7, 3, Piece::QUEEN, Piece::WHITE);
	tab[0][3].setCell(0, 3, Piece::QUEEN, Piece::BLACK);

	tab[7][4].setCell(7, 4, Piece::KING, Piece::WHITE);
	tab[0][4].setCell(0, 4, Piece::KING, Piece::BLACK);
}
