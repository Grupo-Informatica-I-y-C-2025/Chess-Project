//boardgl.h: contains the painting functions for the board
#ifndef __BOARD_GL_H__
#define __BOARD_GL_H__

#pragma once
#include <string>
#include "freeglut.h"
#include "Game.h"

#define PAWN 0
#define KNIGHT 1
#define BISHOP 2
#define ROOK 3
#define QUEEN 4
#define KING 5
#define EMPTY_CELL 6

#define OWHITE 0
#define OBLACK 1
#define ONONE 2

//enums to eliminate dependency of glut
enum { MOUSE_LEFT_BUTTON, MOUSE_MIDDLE_BUTTON, MOUSE_RIGHT_BUTTON };
enum { KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };

using namespace std;
struct CaptureAnimation {
    int x, y;
    float timer;
};

class BoardGL {
	
	void drawCircle(float cx, float cy, float r, int num_segments);
	std::vector<CaptureAnimation> activeCaptures;
	
	
public:
	//define board 
	BoardGL( Game g) : game(g) {
		board = g.board;
		width = 0.15;				//width of each cell in the grid
		N = g.board->getSizeY();
		M = g.board->getSizeX();		//Grid NxN
		dist = 2;					//distance of viewpoint from center of the board
		center_z = 0;
	}
	virtual ~BoardGL() {}

	//Initizalization
	void init();						//enable lights

	//Event managers
	void Draw();
	void DrawGrid();
	void DrawCell(int , int ,int );
	void KeyDown(unsigned char key);
	//void SpecialKeyDown(unsigned char key);
	void MouseButton(int x, int y, int button, bool down, bool shiftKey, bool ctrlKey);
	void OnTimer(int value);
	void registerCapture(int x, int y);
	

	Bitboard attackMap=0;

	//info
	void setSize(int s) { N = s; }
	int getSize() { return N; }

	//coord
	void cell2center(int fila, int columna, float& glx, float& gly) {
		glx = columna * width + width / 2.0f;
		gly = fila * width + width / 2.0f;
	}

	void world2cell(double x, double y, int& cell_x, int& cell_y) {
		cell_x = static_cast<int>(y / width); 
		cell_y = static_cast<int>(x / width);
	}

protected:
	int countBack=0;

	void countBackUp() {if (countBack > 0)countBack--;}
	void countBackDown() {if (countBack < board->moveCount) countBack++;}
	void attackPathActivation();

	void DrawBorder();
	void DrawRecorder();

	float width;
	int N;
	int M;//size 
	Board* board;
	Game game;

	//visualization	
	double center_x, center_y, center_z;
	double dist;

	//mouse	
	int xcell_sel, ycell_sel;			//cell selected with mouse

	//mouse+special-keys combination state events 
	bool controlKey;
	bool shiftKey;
	bool leftButton;
	bool rightButton;
	bool midButton;
};
void initModels();                             // Inicializa todas las texturas


#endif