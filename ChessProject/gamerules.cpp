#include "Game.h"
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

//mover a boardGL
void Game::unselectAll() {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			board->getTab()[i][j].unselectCell();
			action = 0;
		}
	}
}
void Game::activate( int xcell_sel, int ycell_sel)
{
	////Activar casillas -- parte propia	
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < N)) {
		if (board->getTab()[xcell_sel][ycell_sel].selected())
		{
			board->getTab()[xcell_sel][ycell_sel].unselectCell();
			action = 0;
			src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		}
		else if (board->getTab()[xcell_sel][ycell_sel].getColor() == turn)
		{
			unselectAll();
			board->getTab()[xcell_sel][ycell_sel].selectCell();
			action = 1;
		}
	}

}
void Game::registerCall(int xcell_sel, int ycell_sel) {
	if ((xcell_sel >= 0 && xcell_sel < N) && (ycell_sel >= 0 && ycell_sel < N)) {
		if (!action)
		{
			src[0] = xcell_sel, src[1] = ycell_sel;
		}
		else {
			dest[0] = xcell_sel, dest[1] = ycell_sel;
		}
	}
}



//reorganizar las funciones de juego, limpiar
board_piece Game::findPiece(int x, int y) {
	for (const auto& p : board->board_pieces) {
		if (x == p.x && y == p.y) {
			return p;
		}
	}
	board_piece null{ -1,-1 };
	return null;
}
void Game::makeMove() {
		
	if (!scanCheckMate(turn) && turn == bot) {
		auto botmove = botMove(turn);
		src[0] = botmove.source.y ; src[1] = botmove.source.x ; dest[0]= botmove.destination.y ; dest[1] = botmove.destination.x ;
		//cout << "\n move:" << src[0] << src[1] << dest[0] << dest[1];
	}

	 board_piece piece = findPiece( src[1], src[0]);
	if (piece.x != -1 && piece.y != -1 && dest[0] != -1 && dest[1] != -1) {
		movement move = { piece,{dest[0],dest[1],board->getTab()[dest[0]][dest[1]].getColor(),board->getTab()[dest[0]][dest[1]].getType() } };
		//cout << "\n move:" << piece.y << piece.x << dest[0] << dest[1];
		if (piece.color == turn && board->isLegalmove(move) && doMove( move))changeTurn();
		else { src[0] = dest[0]; src[1] = dest[1]; }
		
		dest[0] = -1, dest[1] = -1;
	}
	
}
bool Game::doMove(movement& move) {//
	vector<board_piece> null;
	board_piece ep = enPassant(board, move);
	board->updateBoardPiece(move.destination.x, move.destination.y, move.source.type, move.source.color);
	board->updateBoardPiece(move.source.x , move.source.y, EMPTY_CELL, NONE);
	

	if (scanChecks(board,move.source.color,null)) {
		//cout << "\nmove invalid because resulted in check";
		//redo en passant
		if (ep.x != -1 && ep.y != -1)board->updateBoardPiece(ep.x, ep.y, ep.type, ep.color);

		board->updateBoardPiece(move.destination.x , move.destination.y, move.destination.type, move.destination.color);
		board->updateBoardPiece(move.source.x,move.source.y, move.source.type, move.source.color);
		return false;
	}
	return true;
}
void Game::changeTurn() {//referencia a cambiar de turno o traer turno <aqui
	vector<board_piece> null;
	if (turn == BLACK)turn = WHITE;
	else turn = BLACK;

	if (!scanCheckMate(turn)) {
		if (scanChecks(board, turn,null)) {
			cout << "\n check!";
		}
		if (turn == WHITE)cout << "\nwhites turn score:   "<<EvaluateGame(board);
		else cout << "\nblacks turn score:   " << EvaluateGame(board);
	}else{ game_over = true; }

	

	//reset
	cleanEnpassant(board);
	action = 0;
	unselectAll();
}

//implementacion de en passant en el juego
board_piece Game::enPassant(Board* board,const movement& pawn) {//
	if (board->getTab()[pawn.source.y][pawn.source.x].getType() == PAWN) {
		if ((pawn.source.y == board->getSizeY() - 2) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y - 2)) {
			//aquí hay que insertar un flag para el en passant
			board->getTab()[pawn.destination.y + 1][pawn.destination.x].setFlag(pawn.destination.y + 1, pawn.destination.x, Object::WHITE_EP);
		}
		else if ((pawn.source.y == 1) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y + 2)) {
			//aquí hay que insertar un flag para el en passant
			board->getTab()[pawn.destination.y - 1][pawn.destination.x].setFlag(pawn.destination.y - 1, pawn.destination.x, Object::BLACK_EP);
		}

		if (board->getTab()[pawn.destination.y][pawn.destination.x].getFlag() == Object::WHITE_EP) {
			board_piece ep = { pawn.destination.y - 1, pawn.destination.x , board->getTab()[pawn.destination.y-1][pawn.destination.x].getColor(), board->getTab()[pawn.destination.y-1][pawn.destination.x].getType() };
			board->updateBoardPiece(pawn.destination.x, pawn.destination.y - 1, EMPTY_CELL, NONE);
			return  ep ;
		}
		if (board->getTab()[pawn.destination.y][pawn.destination.x].getFlag() == Object::BLACK_EP) {
			board_piece ep = { pawn.destination.y + 1, pawn.destination.x , board->getTab()[pawn.destination.y + 1][pawn.destination.x].getColor(), board->getTab()[pawn.destination.y+1][pawn.destination.x].getType() };
			board->updateBoardPiece(pawn.destination.x, pawn.destination.y + 1, EMPTY_CELL, NONE);
			return ep;
		}
		return pawn.source;
	}
	board_piece null{ -1,-1 };
	return null;
}
void  Game::unPassant(Board* board, const movement& pawn, const board_piece& ep) {//
	if (ep.x != -1 && ep.y != -1)board->updateBoardPiece(ep.x, ep.y, ep.type, ep.color);
	if ((pawn.source.y == board->getSizeY() - 2) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y - 2) /*&& (pawn.destination.color == NONE)  && board->getTab()[pawn.destination.y + 1][pawn.destination.x].getColor() == NONE*/) {
		//aquí hay que insertar un flag para el en passant
		board->getTab()[pawn.destination.y + 1][pawn.destination.x].setFlag(pawn.destination.y + 1, pawn.destination.x, Object::NO_FLAG);
	}
	else if ((pawn.source.y == 1) && (pawn.source.x == pawn.destination.x) && (pawn.destination.y == pawn.source.y + 2) /* && (pawn.destination.color == NONE) && (getTab()[pawn.destination.y - 1][pawn.destination.x].getColor() == NONE)*/) {
		//aquí hay que insertar un flag para el en passant
		board->getTab()[pawn.destination.y - 1][pawn.destination.x].setFlag(pawn.destination.y - 1, pawn.destination.x, Object::NO_FLAG);
	}
}
void Game::cleanEnpassant(Board* board) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			if (turn == WHITE && board->getTab()[i][j].getFlag() == Object::WHITE_EP) {
					board->getTab()[i][j].setFlag(i, j, Object::NO_FLAG);
					//break;
				}
			if (turn == BLACK && board->getTab()[i][j].getFlag() == Object::BLACK_EP) {
					board->getTab()[i][j].setFlag(i, j, Object::NO_FLAG);
					//break;
				}

			}

		}
	}

//seguir optimizando scancheck para guardar los bitboard en caché
void Game::updateAttackTables() {

	whiteAttackTable.assign(N, vector<bool>(N, false));
	blackAttackTable.assign(N, vector<bool>(N, false));

	for (int y = 0; y < N; y++) {
		for (int x = 0; x < N; x++) {
			if (board->getTab()[y][x].getColor() != NONE) {
				calculatePieceAttacks(y, x);
			}
		}
	}
}
void Game::calculatePieceAttacks(int y, int x) {
	Object::color_t color = board->getTab()[y][x].getColor();
	Object::type_t type = board->getTab()[y][x].getType();
	vector<vector<bool>>& attackTable = (color == WHITE) ? whiteAttackTable : blackAttackTable;
	bool Kramnik = board->isKramnik();

	switch (type) {

	case PAWN:

		if (color == WHITE) {
			if (y > 0) {
				if (x > 0) attackTable[y - 1][x - 1] = true;
				if (x < N - 1) attackTable[y - 1][x + 1] = true;
			}
		}
		if (color == BLACK) {
			if (y < N - 1) {
				if (x > 0) attackTable[y + 1][x - 1] = true;
				if (x < N - 1) attackTable[y + 1][x + 1] = true;
			}
		}
		break;

	case ROOK:
		// Horizontal izquierda
		for (int i = x - 1; i >= 0; --i) {
			attackTable[y][i] = true;
			if (board->getTab()[y][i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y][i].getColor() == color) break;
				else break;
			}
		}
		// Horizontal derecha
		for (int i = x + 1; i < N; ++i) {
			attackTable[y][i] = true;
			if (board->getTab()[y][i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y][i].getColor() == color) break;
				else break;
			}
		}
		// Vertical arriba
		for (int i = y - 1; i >= 0; --i) {
			attackTable[i][x] = true;
			if (board->getTab()[i][x].getColor() != NONE) {
				if (!Kramnik && board->getTab()[i][x].getColor() == color) break;
				else break;
			}
		}
		// Vertical abajo
		for (int i = y + 1; i < N; ++i) {
			attackTable[i][x] = true;
			if (board->getTab()[i][x].getColor() != NONE) {
				if (!Kramnik && board->getTab()[i][x].getColor() == color) break;
				else break;
			}
		}
		break;

	case BISHOP:
		// Diagonal superior izquierda
		for (int i = 1; y - i >= 0 && x - i >= 0; ++i) {
			attackTable[y - i][x - i] = true;
			if (board->getTab()[y - i][x - i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y - i][x - i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal superior derecha
		for (int i = 1; y - i >= 0 && x + i < N; ++i) {
			attackTable[y - i][x + i] = true;
			if (board->getTab()[y - i][x + i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y - i][x + i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal inferior izquierda
		for (int i = 1; y + i < N && x - i >= 0; ++i) {
			attackTable[y + i][x - i] = true;
			if (board->getTab()[y + i][x - i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y + i][x - i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal inferior derecha
		for (int i = 1; y + i < N && x + i < N; ++i) {
			attackTable[y + i][x + i] = true;
			if (board->getTab()[y + i][x + i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y + i][x + i].getColor() == color) break;
				else break;
			}
		}
		break;

	case KNIGHT:

		if (x + 1 < N && y + 2 < N) attackTable[y + 2][x + 1] = true;
		if (x - 1 >= 0 && y + 2 < N) attackTable[y + 2][x - 1] = true;
		if (x + 1 < N && y - 2 >= 0) attackTable[y - 2][x + 1] = true;
		if (x - 1 >= 0 && y - 2 >= 0) attackTable[y - 2][x - 1] = true;

		if (y + 1 < N && x + 2 < N) attackTable[y + 1][x + 2] = true;
		if (y - 1 >= 0 && x + 2 < N) attackTable[y - 1][x + 2] = true;
		if (y + 1 < N && x - 2 >= 0) attackTable[y + 1][x - 2] = true;
		if (y - 1 >= 0 && x - 2 >= 0) attackTable[y - 1][x - 2] = true;
		break;

	case QUEEN:

		// Horizontal izquierda
		for (int i = x - 1; i >= 0; --i) {
			attackTable[y][i] = true;
			if (board->getTab()[y][i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y][i].getColor() == color) break;
				else break;
			}
		}
		// Horizontal derecha
		for (int i = x + 1; i < N; ++i) {
			attackTable[y][i] = true;
			if (board->getTab()[y][i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y][i].getColor() == color) break;
				else break;
			}
		}
		// Vertical arriba
		for (int i = y - 1; i >= 0; --i) {
			attackTable[i][x] = true;
			if (board->getTab()[i][x].getColor() != NONE) {
				if (!Kramnik && board->getTab()[i][x].getColor() == color) break;
				else break;
			}
		}
		// Vertical abajo
		for (int i = y + 1; i < N; ++i) {
			attackTable[i][x] = true;
			if (board->getTab()[i][x].getColor() != NONE) {
				if (!Kramnik && board->getTab()[i][x].getColor() == color) break;
				else break;
			}
		}
		// Diagonal superior izquierda
		for (int i = 1; y - i >= 0 && x - i >= 0; ++i) {
			attackTable[y - i][x - i] = true;
			if (board->getTab()[y - i][x - i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y - i][x - i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal superior derecha
		for (int i = 1; y - i >= 0 && x + i < N; ++i) {
			attackTable[y - i][x + i] = true;
			if (board->getTab()[y - i][x + i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y - i][x + i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal inferior izquierda
		for (int i = 1; y + i < N && x - i >= 0; ++i) {
			attackTable[y + i][x - i] = true;
			if (board->getTab()[y + i][x - i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y + i][x - i].getColor() == color) break;
				else break;
			}
		}
		// Diagonal inferior derecha
		for (int i = 1; y + i < N && x + i < N; ++i) {
			attackTable[y + i][x + i] = true;
			if (board->getTab()[y + i][x + i].getColor() != NONE) {
				if (!Kramnik && board->getTab()[y + i][x + i].getColor() == color) break;
				else break;
			}
		}
		break;

	case KING:

		if (x + 1 < N && y + 1 < N) attackTable[y + 1][x + 1] = true;
		if (x + 1 < N && y - 1 >= 0) attackTable[y - 1][x + 1] = true;
		if (y + 1 < N && x - 1 >= 0) attackTable[y + 1][x - 1] = true;
		if (y - 1 >= 0 && x - 1 >= 0) attackTable[y - 1][x - 1] = true;

		if (y + 1 < N) attackTable[y + 1][x] = true;
		if (y - 1 >= 0) attackTable[y - 1][x] = true;
		if (x + 1 < N) attackTable[y][x + 1] = true;
		if (x - 1 >= 0) attackTable[y][x - 1] = true;
		break;

	case EMPTY_CELL:
		break;

	default:
		break;
	}
}
bool Game::findKing(Board* board, Object::color_t c) {
	for (const auto& p : board->board_pieces) {
		if (p.type == KING && p.color == c) {
			board->kx = p.x; board->ky = p.y;
			return true;
		}
	}
	return false;
}
bool Game::scanChecks(Board* board, Object::color_t color,vector<board_piece>& attackers) {
	attackers.clear();
	board->listPieces();
	if (!findKing(board, color)) return true;

	updateAttackTables();
	const auto& attackTable = (color == WHITE) ? blackAttackTable : whiteAttackTable;

	if (attackTable[board->ky][board->kx]) {
		for (const auto& p : board->board_pieces) {
			if (p.color != color) {
				movement move = { p,{board->ky,board->kx,color,KING} };
				if (board->isLegalmove(move))attackers.push_back(p);
			}
		}
		return true;
	}

	//ray attacks

	return false;
}

//funciones que optimizan la detccion de jaque mate
bool Game::canKingMove(Board *board,Object::color_t color) {
	//8 posibles direcciones de movimiento
	//board->listPieces();
	findKing(board, color);
	static const int dirs[8][2] = { {-1,-1}, {-1,0}, {-1,1}, {0,-1}, {0,1}, {1,-1}, {1,0}, {1,1} };
	vector<board_piece> null;
	int original_ky = board->ky;
	int original_kx = board->kx;
	for (auto& d : dirs) {
		int ny = board->ky + d[0];
		int nx = board->kx + d[1];
		if (ny < 0 || ny >= N || nx < 0 || nx >= M) continue;
		
		movement move{ {original_ky, original_kx, color, KING}, {ny, nx, board->getTab()[ny][nx].getColor(), board->getTab()[ny][nx].getType()} };
		if (board->isLegalmove(move)) {
			//hace movimiento
			board->updateBoardPiece(move.destination.x, move.destination.y, move.source.type, move.source.color);
			board->updateBoardPiece(move.source.x, move.source.y, EMPTY_CELL, NONE);

			bool check = scanChecks(board,color, null);

			board->updateBoardPiece(move.destination.x, move.destination.y, move.destination.type, move.destination.color);
			board->updateBoardPiece(move.source.x, move.source.y, move.source.type, move.source.color);
			board->ky = original_ky;
			board->kx = original_kx;
			//board->listPieces();
			if (!check)return true;

		}
	}
	return false;
}
bool Game::canCaptureAttacker(Board* board, Object::color_t color, const board_piece& attacker) {
	//board->listPieces();
	vector<board_piece> null;
	for (const auto& p : board->board_pieces) {
		if (p.color == color && p.type!=KING ) {

			movement move = { p,{attacker.y,attacker.x,attacker.color,attacker.type} };
			if (board->isLegalmove(move)) {
				board_piece ep = enPassant(board, move);
				board->updateBoardPiece(move.destination.x, move.destination.y, move.source.type, move.source.color);
				board->updateBoardPiece(move.source.x, move.source.y, EMPTY_CELL, NONE);

				bool check = scanChecks(board, color, null);

				unPassant(board, move, ep);
				board->updateBoardPiece(move.destination.x, move.destination.y, move.destination.type, move.destination.color);
				board->updateBoardPiece(move.source.x, move.source.y, move.source.type, move.source.color);
				//board->listPieces();
				if (!check)return true;
			}
		}
	}
	return false;
}
bool Game::canBlockAttacker(Board* board, Object::color_t color, const board_piece& attacker) {
	//board->listPieces();
	vector<board_piece> null;
	int dy = (attacker.y - board->ky) ? (attacker.y - board->ky) / abs(attacker.y - board->ky) : 0;
	int dx = (attacker.x - board->kx) ? (attacker.x - board->kx) / abs(attacker.x - board->kx) : 0;

	vector<pair<int,int>> path;
	int y = board->ky + dy;
	int x = board->kx + dx;

	while (y != attacker.y || x != attacker.x) {
		path.emplace_back( y, x );
		y += dy;
		x += dx;
	}
	for (auto& pos : path) {
		for (const auto& p : board->board_pieces) {
			movement move = { p,{pos.first,pos.second,board->getTab()[pos.first][pos.second].getColor() ,board->getTab()[pos.first][pos.second].getType()} };
			if (board->isLegalmove(move)) {
				board_piece ep = enPassant(board, move);
				board->updateBoardPiece(move.destination.x, move.destination.y, move.source.type, move.source.color);
				board->updateBoardPiece(move.source.x, move.source.y, EMPTY_CELL, NONE);

				bool check = scanChecks(board, color, null);

				unPassant(board, move, ep);
				board->updateBoardPiece(move.destination.x, move.destination.y, move.destination.type, move.destination.color);
				board->updateBoardPiece(move.source.x, move.source.y, move.source.type, move.source.color);
				//board->listPieces();
				if (!check)return true;
			}
		}
	}
	return false;
}
bool Game::scanCheckMate(Object::color_t turn) {
	vector<board_piece> attackers;
	bool check = scanChecks(board, turn, attackers);
	if (!check)return false;//si no hay jaque
	if (attackers.size() > 1) return !canKingMove(board, turn);//si es doble jaque

	if (canKingMove(board, turn))return false;//si puede moverse el rey

	if (canCaptureAttacker(board, turn, attackers[0]))return false;//si se puede comer al atacante

	if (attackers[0].type == QUEEN || attackers[0].type == ROOK || attackers[0].type == BISHOP) {//si se puede bloquear el atacante
		if (canBlockAttacker(board, turn, attackers[0]))return false;
	}


	return true;
}
