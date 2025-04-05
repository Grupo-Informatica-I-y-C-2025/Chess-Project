//game.cpp: entry point for a general outline of baordgame using glut and OpenGL
//			configures basic glut parameters and defines a scene wrapper for all 
//			call backs
//author:pss
//date of creation: 9/5/16
#include "freeglut.h"
#include <iostream>
#include "boardgl.h"
#pragma once


#define GRID_SIZEY	8	
#define GRID_SIZEX  8	//NxN grid
#define KRAMNIK 0

/////////////////////////////////
//call back declarations: will be called by glut when registered
void OnDraw(void);
void OnKeyboardDown(unsigned char key, int x, int y);
void OnMouseClick(int button, int state, int x, int y);
void OnTimer(int value); 
//void motion(int x, int y);

////////////////////////////////////////////////
//global objects which make the world
Board gameboard(GRID_SIZEY,GRID_SIZEX, KRAMNIK);
Game game(&gameboard);
BoardGL scene(&gameboard,game);



///////////////////////////////////////////////

int main(int argc, char* argv[]) {

	//GL Initialization stuff
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("MiJuego");



	glutTimerFunc(25,OnTimer,0);			 //set timer if required, currently not used
	glutDisplayFunc(OnDraw);
	glutKeyboardFunc(OnKeyboardDown);
	glutMouseFunc(OnMouseClick);
	//glutMotionFunc(motion);
	
	//sets light and perspective
	scene.init();

	//glut takes control
	glutMainLoop();

	return 0;
}

void OnDraw(void) {
	//////////////////////
	//captures drawing event
	//gives control to scene
	scene.Draw();
	glutSwapBuffers();
}

void OnKeyboardDown(unsigned char key, int x_t, int y_t) {
	//////////////////////
	//captures keyboard event
	//gives control to scene
	scene.KeyDown(key);
	glutPostRedisplay();
}

void OnMouseClick(int b, int state, int x, int y) {
	//////////////
	//captures clicks with mouse with or without special keys (CTRL or SHIFT)
	//gives control to board scene
	bool down = (state == GLUT_DOWN);
	int button;
	if (b == GLUT_LEFT_BUTTON) {
		button = MOUSE_LEFT_BUTTON;
	}
	if (b == GLUT_RIGHT_BUTTON) {
		button = MOUSE_RIGHT_BUTTON;
		cout << "MOUSE_RIGHT_BUTTON" << endl;
	}

	int specialKey = glutGetModifiers();
	bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
	bool sKey = specialKey & GLUT_ACTIVE_SHIFT;



	//
	scene.MouseButton(x, y, b, down, sKey, ctrlKey);
	

	glutPostRedisplay();
}

/*void motion(int x, int y) {
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

	

	
	for (int i = 0; i < GRID_SIZE; i++) {
		for (int j = 0;j < GRID_SIZE; j++) {
			if (gameboard.getTab()[i][j].draged()) {
				scene.DrawCell(i, j, posX, posY);
				
			}
		}
	}
	

}*/

void OnTimer(int value) {
	

	////Activa movimiento
	
	scene.OnTimer(value);

	//sets new timer (do not delete)
	glutTimerFunc(25,OnTimer,0);
	glutPostRedisplay();
}
