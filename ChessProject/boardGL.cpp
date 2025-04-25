#include "freeglut.h"
#include "gltools.h"
#include "boardgl.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>

#define DEG2RAD M_PI/180.0			//not used



void BoardGL::init() {
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0, 800 / 600.0f, 0.1, 150);
}

void BoardGL::DrawGrid() {
	//////////////////////
	// Draws NxN grid of size width with the upper left hand corner
	// at (0,0)



	GLTools::Color(gltools::BLACK);
	float dist1 = M * width;
	for (int i = 0; i <= N; i++) {
		if (i % N == 0) glLineWidth(4);
		else glLineWidth(1);
		glBegin(GL_LINES);

		glVertex3f(0, i * width, 0);    
		glVertex3f(dist1, i * width, 0);
		glEnd();
	}
	float dist2 = N * width;
	for (int i = 0; i <= M; i++) {
		if (i % M == 0) glLineWidth(4);
		else glLineWidth(1);
		glBegin(GL_LINES);

		glVertex3f(i * width, 0, 0);           
		glVertex3f(i * width, dist2, 0);
		glEnd();
	}
	
}

void BoardGL::DrawCell(int y, int x) {
	////////////////////
	// Draws contents of cell	
	// Note:(0, 0) screen coordinates is the upper left hand corner of the board
	//
	// INPUT: (i, j) are cell coordinates, (0,0) is the upper left hand square of the board// 

	float glx, gly;
	cell2center(y, x, glx, gly);

	//Color gris si la pieza esta seleccionada
	if (board->BitboardGetColor(x,y) == OWHITE)
	{
		if (board->selected(x,y))GLTools::Color(gltools::GREY);//subraya si seleccionado
		else GLTools::Color(gltools::WHITE);
	}
	if (board->BitboardGetColor(x, y) == OBLACK)
	{
		if (board->selected(x,y))GLTools::Color(gltools::GREY);//subraya si seleccionado
		else GLTools::Color(gltools::BLACK);
	}

	switch (board->BitboardGetType(x, y)) {

	case PAWN:
		glDisable(GL_LIGHTING);

		//Dibuja el peón

		glTranslatef(glx, gly, 0);
		drawPawn();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case ROOK:
		glDisable(GL_LIGHTING);

		//Dibuja la torre

		glTranslatef(glx, gly, 0);
		drawRook();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case BISHOP:
		glDisable(GL_LIGHTING);

		//Dibuja el alfil

		glTranslatef(glx, gly, 0);
		drawBishop();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case KNIGHT:
		glDisable(GL_LIGHTING);

		//Dibuja el alfil

		glTranslatef(glx, gly, 0);
		drawKnigth();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case QUEEN:
		glDisable(GL_LIGHTING);

		//Dibuja el alfil

		glTranslatef(glx, gly, 0);
		drawQueen();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case KING:
		glDisable(GL_LIGHTING);

		//Dibuja el alfil

		glTranslatef(glx, gly, 0);
		drawKing();
		glTranslatef(-glx, -gly, 0);

		glEnable(GL_LIGHTING);
		break;

	case EMPTY_CELL:break;

	default:
		;
	}

	if (attackMap & 1ULL << y * 8 + x) {
		GLTools::Color(gltools::GREY);
		glTranslatef(glx, gly, 0);
		drawCircle(0,0, width / 7.0f, 50);
		glTranslatef(-glx, -gly, 0);

	}


}

void BoardGL::Draw() {

	center_x = M * width / 2;
	center_y = N * width / 2;
	center_z = 0;

	//Borrado de la pantalla	
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(center_x, center_y, dist, center_x, center_y, center_z, 0, 1, 0);
	glEnable(GL_LIGHTING);


	if (board->getSelected()) {
		int source = board->portable_ctzll(board->getSelected());
		int pieceType = board->BitboardGetType(source);
		Bitboard movesBB  = board->generateMovesFrom(source, board->BitboardGetTurn());
		while (movesBB) {
			int target = board->portable_ctzll(movesBB);
			movesBB &= movesBB - 1;
			Move move = { source, target, pieceType, DEFAULT };

			if (board->isLegalmove(move))attackMap|=1ULL<<target;
		}

	}
	else attackMap = 0;



	//Draws board and grid
	DrawGrid();
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			DrawCell(i, j);
		}
	}

	//Draw a transparent rectangle over the board to capture mouse input
	//with gluUnProject
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLTools::Color(gltools::RED, 1.0f);
	glTranslatef(center_x, center_y, center_z);
	glRectf(M * width / 2.0f, N * width / 2.0f, -M * width / 2.0f, -N * width / 2.0f);
	glTranslatef(-center_x, -center_y, -center_z);
	glEnable(GL_LIGHTING);


}

void BoardGL::MouseButton(int x, int y, int button, bool down, bool sKey, bool ctrlKey) {
	/////////////////////////
	// sets state of mouse control buttons + special keys

	/////////
	//computes cell coordinates from mouse coordinates

	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLfloat winX, winY, winZ;
	GLdouble posX, posY, posZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	winX = (float)x;
	winY = (float)viewport[3] - (float)y;
	glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
	gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

	//finally cell coordinates
	world2cell(posX, posY, xcell_sel, ycell_sel);

	///////////////////////////	
	//capture other mouse events

	if (down) {
		controlKey = ctrlKey;
		shiftKey = sKey;
	}
	else {
		controlKey = shiftKey = false;
	}

	if (button == MOUSE_LEFT_BUTTON)
		leftButton = down;
	else if (button == MOUSE_RIGHT_BUTTON)
		rightButton = down;
	else if (button == MOUSE_MIDDLE_BUTTON)
		midButton = down;
	///////////////////////////


	if (down) {

		////Regsitrar psoicion previa y final -- parte propia
		//game.registerCall(xcell_sel, ycell_sel);

		////Iniciar movimiento
		game.activate(xcell_sel, ycell_sel);

	}
}


void BoardGL::KeyDown(unsigned char key) {
	//////////////////
	// enlarges or reduces grid by one

	if (key == 's') {
		//** do something
		//if(N<=8) N++;
	}

	if (key == 'a') {
		//** do something
		//if(N>=2) N--;
	}
}

void BoardGL::OnTimer(int value) {
	if(!game.isgame_over())game.playTurn();
}

