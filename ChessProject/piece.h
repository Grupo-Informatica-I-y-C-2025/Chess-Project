//piece.h: interface for the Piece class Hierarchy and definitions

#include "freeglut.h"
#include <iostream>


using namespace std;


class Object {
	//interface class for all pieces
public:
	enum flag_t { WHITE_EP, BLACK_EP, NO_FLAG };
	enum type_t { PAWN, ROOK, BISHOP, KNIGHT, QUEEN, KING, EMPTY_CELL };
	enum color_t { WHITE, BLACK, NONE };
	virtual void getCellNumber(int& x, int& y) = 0;
	virtual type_t getType() = 0;
};


class Piece :public Object {
public:

protected:
	int x;
	int y;
	type_t type;
	color_t color;
	flag_t flag;
public:
	bool select = 0;
	Piece() :x(-1), y(-1), type(EMPTY_CELL), color (NONE),flag(NO_FLAG) {}
	void setCell(int x, int y, type_t t, color_t col) { this->x = x; this->y = y; this->type = t; this->color = col; }
	void setFlag(int x, int y, flag_t f) { this->x = x; this->y = y; this->flag = f; }
	virtual void getCellNumber(int& x, int& y) { x = this->x; y = this->y; }
	virtual type_t getType() { return type; }
	virtual color_t getColor() { return color; }
	virtual flag_t getFlag() { return flag; }
	virtual void selectCell(){ select = 1; }
	virtual void unselectCell() {select = 0;}
	virtual bool selected() { return select; }

};


