
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
	float dist = N * width;
	for (int i = 0; i <= N; i++) {
		if (i % N == 0) glLineWidth(4);
		else glLineWidth(1);
		glBegin(GL_LINES);
		glVertex3f(i * width, 0, 0);
		glVertex3f(i * width, -dist, 0);
		glVertex3f(0, -i * width, 0);
		glVertex3f(dist, -i * width, 0);
		glEnd();
	}
}

void BoardGL::DrawCell(int i, int j) {
	////////////////////
	// Draws contents of cell	
	// Note:(0, 0) screen coordinates is the upper left hand corner of the board
	//
	// INPUT: (i, j) are cell coordinates, (0,0) is the upper left hand square of the board// 
	float glx, gly;
	switch (m_board->getTab()[i][j].getType()) {

	case Object::PAWN:
		cell2center(i, j, glx, gly);
		glDisable(GL_LIGHTING);

		//Color gris si la pieza esta seleccionada
		if (m_board->getTab()[i][j].getColor()==Object::WHITE)
		{
		if(m_board->getTab()[i][j].selected())GLTools::Color(gltools::GREY);//subraya si seleccionado
		else GLTools::Color(gltools::WHITE);
		}
		if(m_board->getTab()[i][j].getColor() == Object::BLACK)
		{
			if (m_board->getTab()[i][j].selected())GLTools::Color(gltools::GREY);//subraya si seleccionado
			else GLTools::Color(gltools::BLACK);
		}

		glTranslatef(glx, gly, 0);

		//Dibuja el peón
		drawPawn(m_board->getTab()[i][j].getColor());

		glTranslatef(-glx, -gly, 0);
		glEnable(GL_LIGHTING);
		break;

	case Object::EMPTY_CELL:
	default:
		;
	}
}

void BoardGL::Draw() {

	center_x = N * width / 2;
	center_y = -N * width / 2;
	center_z = 0;

	//Borrado de la pantalla	
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(center_x, center_y, dist, center_x, center_y, center_z, 0, 1, 0);
	glEnable(GL_LIGHTING);

	//Draws board and grid

	DrawGrid();
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
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
	glRectf(N * width / 2.0f, N * width / 2.0f, -N * width / 2.0f, -N * width / 2.0f);
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

		//***WRITE ACTIONS CONNECTED TO MOUSE STATE HERE

	
	if (down) {

		////Regsitrar psoicion previa y final -- parte propia
		registerCall();

		////Iniciar movimiento
		activate();

		////Activa movimiento
		
		makeMove();

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


	if (src[0] != -1 && src[1] != -1 && dest[0] != -1 && dest[1] != -1 ) {
		if ((m_board->getTab()[src[0]][src[1]].getColor() == m_board->getTab()[dest[0]][dest[1]].getColor()) && (m_board->getTab()[dest[0]][dest[1]].getColor() != Object::NONE))
		{
			return false;
		}

		else if (m_board->getTab()[src[0]][src[1]].getColor() == turn){


			switch (m_board->getTab()[src[0]][src[1]].getType())
			{
			case Object::PAWN:
				cout << "moving pawn \n";
				pawnMove();
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
		cout << "S: " << src[0] << " " << src[1] << "\n";
		cout << "D: " << dest[0] << " " << dest[1] << "\n";
		src[0] = -1, src[1] = -1, dest[0] = -1, dest[1] = -1;
		unselectAll();

	}
}

void BoardGL::doMove() {
	//mueve la pieza de src a dest, vacía src y desactiva la acción
	m_board->getTab()[dest[0]][dest[1]].setCell(dest[0], dest[1], Piece::PAWN, m_board->getTab()[src[0]][src[1]].getColor());
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
			doMove();
			changeTurn();
			return true;
		}
		else
			if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] - 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::BLACK)
			{
				//comida diagonal
				doMove();
				changeTurn();
				return true;
			}
			else
				return false;
	}
	else
		if (m_board->getTab()[src[0]][src[1]].getColor() == Object::BLACK)
		{
			if (src[1] == dest[1] && dest[0] == src[0] + 1 && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::NONE)
			{
				//movimiento recto
				doMove();
				changeTurn();
				return true;
			}
			else
				if ((src[1] + 1 == dest[1] || src[1] - 1 == dest[1]) && src[0] + 1 == dest[0] && m_board->getTab()[dest[0]][dest[1]].getColor() == Object::WHITE)
				{
					//comida diagonal
					doMove();
					changeTurn();
					return true;
				}
				else
					return false;
		}
		else
			return false;
}

void BoardGL::drawPawn(Object::color_t c)
{
	if (c == Object::WHITE) {
		//cabeza del peón
		glTranslatef(0, width / 4, 0);
		glutSolidSphere((width / 5.0f) * 0.9f, 50, 50);
		glTranslatef(0, -width / 4, 0);

		//cuerpo del peón

		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, -(width / 5.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(-(width / 5.0f) * 0.9f, 0 , 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, -(width / 10.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef((width / 5.0f) * 0.9f, 0, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef((width / 5.0f) * 0.9f, 0, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, (width / 10.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(-(width / 5.0f) * 0.9f, 0, 0);
		glTranslatef(0, (width / 5.0f) * 0.9f, 0);

	}
	else if (c == Object::BLACK) {
		//cabeza del peón
		glTranslatef(0, -width / 4, 0);
		glutSolidSphere((width / 5.0f) * 0.9f, 50, 50);
		glTranslatef(0, +width / 4, 0);

		//cuerpo del peón

		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, (width / 5.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef((width / 5.0f) * 0.9f, 0, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, (width / 10.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(-(width / 5.0f) * 0.9f, 0, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(-(width / 5.0f) * 0.9f, 0, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef(0, -(width / 10.0f) * 0.9f, 0);
		glutSolidCube((width / 5.0f) * 0.9f);
		glTranslatef((width / 5.0f) * 0.9f, 0, 0);
		glTranslatef(0, -(width / 5.0f) * 0.9f, 0);

	}
}

