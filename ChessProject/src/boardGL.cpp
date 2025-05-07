#include "freeglut.h"
#include "gltools.h"
#include "boardGL.h"


void BoardGL::Draw() {

	center_x = M * width / 2;
	center_y = N * width / 2;
	center_z = 0;

	//Borrado de la pantalla	
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat light_position[] = {0.0f, 0.0f, 10.0f, 1.0f};
    if (!board->currentState.turn) {
        light_position[1] = -10.0f;  // Ajustar posición si es el turno de las negras
    }
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	bool whiteTurn = (board->BitboardGetTurn() == 0); // o como tengas el sistema

	//if (whiteTurn) {
		gluLookAt(center_x, center_y - 1, dist-4,
			center_x, center_y, center_z,
			0.0f, 1.0f, 0.0f);
	/*}
	else {
		gluLookAt(center_x, center_y +1, dist-4,  // punto de vista opuesto
			center_x, center_y, center_z,
			0.0f, -1.0f, 0.0f);
	}*/
	
	glEnable(GL_LIGHTING);

	//Dibujar piezas, tablero y casillas
	
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			DrawCell(i, j, countBack);
		}
	}
	DrawGrid();

	DrawBorder();
	
	DrawRecorder();
	


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


	if (down && !countBack) {

		////Regsitrar psoicion previa y final -- parte propia
		//game.registerCall(xcell_sel, ycell_sel);

		////Iniciar movimiento
		game.activate(xcell_sel, ycell_sel);
		attackPathActivation();
	}
}
void BoardGL::KeyDown(unsigned char key) {
	//////////////////
	// enlarges or reduces grid by one

	if (key == 'a') {
		countBackDown();
	}

	if (key == 'd') {
		countBackUp();
	}
	if (key == 's') {
		game.saveGame();
	}

	if (key == 'l') {
		game.loadSavedGame();
	}
}
void BoardGL::OnTimer(int value) {
	if (!game.isgame_over()) {
		game.playTurn();
	}
}

void BoardGL::attackPathActivation(){
	if (board->getSelected() && !countBack) {
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
}