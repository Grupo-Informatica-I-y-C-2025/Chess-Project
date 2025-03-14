#include "boardGL.h"
#include "gltools.h"

void drawCircle(float cx, float cy, float r, int num_segments) {
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy); // center of circle
	for (int ii = 0; ii <= num_segments; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments); // get the current angle
		float x = r * cosf(theta); // calculate the x component
		float y = r * sinf(theta); // calculate the y component
		glVertex2f(x + cx, y + cy); // output vertex
	}
	glEnd();
}

void BoardGL::drawPawn(Object::color_t c)
{
	if (c == Object::WHITE) {
		//Cabeza
		drawCircle(0, width / 7, width / 7.0f, 50);

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.08, width *0.25, 0.0);
		glVertex3f(width*0.08, width *0.25, 0.0);
		glVertex3f(width *0.08, -width *0.15, 0.0);
		glVertex3f(-width *0.08, -width *0.15, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width *0.12, -width *0.15, 0.0);
		glVertex3f(2 * width *0.1, -width * 0.35, 0.0);
		glVertex3f(-2 * width *0.1, -width * 0.35, 0.0);
		glVertex3f(-width *0.12, -width *0.15, 0.0);
		glEnd();
	}
	if (c == Object::BLACK) {
		//cabeza del peón
		drawCircle(0, -width / 7, width / 7.0f, 50);

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(width *0.08, -width *0.25, 0.0);
		glVertex3f(-width *0.08, -width *0.25, 0.0);
		glVertex3f(-width *0.08, width *0.15, 0.0);
		glVertex3f(width *0.08, width *0.15, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width *0.12, width *0.15, 0.0);
		glVertex3f(-2 * width *0.1, width * 0.35, 0.0);
		glVertex3f(2 * width *0.1, width * 0.35, 0.0);
		glVertex3f(width *0.12, width *0.15, 0.0);
		glEnd();

	}
}

void BoardGL::drawRook(Object::color_t c) {
	if (c == Object::WHITE) {
		
		//Cabeza
		
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, width * 0.25, 0.0);
		glVertex3f(-width * 0.2, width * 0.4, 0.0);
		glVertex3f(-width * 0.11, width * 0.4, 0.0);
		glVertex3f(-width * 0.11, width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.07, width * 0.25, 0.0);
		glVertex3f(-width * 0.07, width * 0.4, 0.0);
		glVertex3f(width * 0.07, width * 0.4, 0.0);
		glVertex3f(width * 0.07, width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.11, width * 0.25, 0.0);
		glVertex3f(width * 0.11, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.25, 0.0);
		glEnd();

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.15, width * 0.25, 0.0);
		glVertex3f(width * 0.15, width * 0.25, 0.0);
		glVertex3f(width * 0.15, -width * 0.25, 0.0);
		glVertex3f(-width * 0.15, -width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.2, -width * 0.25, 0.0);
		glVertex3f(width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.25, 0.0);
		glEnd(); 
	}
	if (c == Object::BLACK) {

		//Cabeza
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, -width * 0.25, 0.0);
		glVertex3f(-width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.11, -width * 0.4, 0.0);
		glVertex3f(-width * 0.11, -width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.07, -width * 0.25, 0.0);
		glVertex3f(-width * 0.07, -width * 0.4, 0.0);
		glVertex3f(width * 0.07, -width * 0.4, 0.0);
		glVertex3f(width * 0.07, -width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.11, -width * 0.25, 0.0);
		glVertex3f(width * 0.11, -width * 0.4, 0.0);
		glVertex3f(width * 0.2, -width * 0.4, 0.0);
		glVertex3f(width * 0.2, -width * 0.25, 0.0);
		glEnd();


		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.15, width * 0.25, 0.0);
		glVertex3f(width * 0.15, width * 0.25, 0.0);
		glVertex3f(width * 0.15, -width * 0.25, 0.0);
		glVertex3f(-width * 0.15, -width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, width * 0.25, 0.0);
		glVertex3f(-width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.25, 0.0);
		glEnd();

	}
}

void BoardGL::drawBishop(Object::color_t c)
{
	if (c == Object::WHITE) {
		//Cabeza
		drawCircle(0, width / 4, width / 7.0f, 50);
		drawCircle(0, width * 0.4, width / 15.0f, 50);

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.08, width * 0.25, 0.0);
		glVertex3f(width * 0.08, width * 0.25, 0.0);
		glVertex3f(width * 0.08, -width * 0.15, 0.0);
		glVertex3f(-width * 0.08, -width * 0.15, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.1, -width * 0.15, 0.0);
		glVertex3f(2 * width * 0.1, -width * 0.35, 0.0);
		glVertex3f(-2 * width * 0.1, -width * 0.35, 0.0);
		glVertex3f(-width * 0.1, -width * 0.15, 0.0);
		glEnd();
	}
	if (c == Object::BLACK) {
		//cabeza del peón

		drawCircle(0, -width / 4, width / 7.0f, 50);
		drawCircle(0, -width * 0.4, width / 15.0f, 50);

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.08, -width * 0.25, 0.0);
		glVertex3f(-width * 0.08, -width * 0.25, 0.0);
		glVertex3f(-width * 0.08, width * 0.15, 0.0);
		glVertex3f(width * 0.08, width * 0.15, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.1, width * 0.15, 0.0);
		glVertex3f(-2 * width * 0.1, width * 0.35, 0.0);
		glVertex3f(2 * width * 0.1, width * 0.35, 0.0);
		glVertex3f(width * 0.1, width * 0.15, 0.0);
		glEnd();

	}
}

void BoardGL::drawKnigth(Object::color_t c) {
	if (c == Object::WHITE) {
		
		//Cabeza
		glBegin(GL_POLYGON);
		glVertex3f(0, width * 0.45, 0.0);
		glVertex3f(width*0.3, width * 0.3, 0.0);
		glVertex3f(0, width * 0.15, 0.0);
		glVertex3f(-width * 0.1, width * 0.3, 0.0);
		glEnd();
		
		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(0, -width * 0.4, 0.0);
		glVertex3f(width * 0.2, -width * 0.1, 0.0);
		glVertex3f(0, width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.1, 0.0);
		glEnd();
		
		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.2, -width * 0.25, 0.0);
		glVertex3f(width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.25, 0.0);
		glEnd();
	}
	if (c == Object::BLACK) {
		glBegin(GL_POLYGON);
		glVertex3f(0, -width * 0.45, 0.0);
		glVertex3f(-width * 0.3, -width * 0.3, 0.0);
		glVertex3f(0, -width * 0.15, 0.0);
		glVertex3f(width * 0.1, -width * 0.3, 0.0);
		glEnd();

		//cuerpo
		glBegin(GL_POLYGON);
		glVertex3f(0, width * 0.4, 0.0);
		glVertex3f(-width * 0.2, width * 0.1, 0.0);
		glVertex3f(0, -width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.1, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, width * 0.25, 0.0);
		glVertex3f(-width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.25, 0.0);
		glEnd();

	}
}

void BoardGL::drawQueen(Object::color_t c) {
	if (c == Object::WHITE) {
		//cabeza
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.14, width * 0.25, 0.0);
		glVertex3f(width * 0.14, width * 0.25, 0.0);
		glVertex3f(width * 0.14, width * 0.4, 0.0);
		glVertex3f(-width * 0.14, width * 0.4, 0.0);
		glEnd();

		//cuerpo
		drawCircle(0, -width *0.15, width/6, 50);

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, -width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.2, -width * 0.25, 0.0);
		glVertex3f(width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.25, 0.0);
		glEnd();
	}
	if (c == Object::BLACK) {
		//cabeza
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.14, -width * 0.25, 0.0);
		glVertex3f(-width * 0.14, -width * 0.25, 0.0);
		glVertex3f(-width * 0.14, -width * 0.4, 0.0);
		glVertex3f(width * 0.14, -width * 0.4, 0.0);
		glEnd();

		//cuerpo
		drawCircle(0, width * 0.15, width / 6, 50);

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, width * 0.25, 0.0);
		glVertex3f(-width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.25, 0.0);
		glEnd();
	}
}

void BoardGL::drawKing(Object::color_t c) {
	if (c == Object::WHITE) {
		//cabeza
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.16, width * 0.25, 0.0);
		glVertex3f(-width * 0.16, width * 0.4, 0.0);
		glVertex3f(-width * 0.09, width * 0.4, 0.0);
		glVertex3f(-width * 0.09, width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.07, width * 0.25, 0.0);
		glVertex3f(-width * 0.07, width * 0.4, 0.0);
		glVertex3f(width * 0.07, width * 0.4, 0.0);
		glVertex3f(width * 0.07, width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.09, width * 0.25, 0.0);
		glVertex3f(width * 0.09, width * 0.4, 0.0);
		glVertex3f(width * 0.16, width * 0.4, 0.0);
		glVertex3f(width * 0.16, width * 0.25, 0.0);
		glEnd();

		//cuerpo
		drawCircle(0, -width * 0.15, width / 6, 50);

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, -width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(width * 0.2, -width * 0.25, 0.0);
		glVertex3f(width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.4, 0.0);
		glVertex3f(-width * 0.2, -width * 0.25, 0.0);
		glEnd();
	}
	if (c == Object::BLACK) {
		//cabeza
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.16, -width * 0.25, 0.0);
		glVertex3f(-width * 0.16, -width * 0.4, 0.0);
		glVertex3f(-width * 0.09, -width * 0.4, 0.0);
		glVertex3f(-width * 0.09, -width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.07, -width * 0.25, 0.0);
		glVertex3f(-width * 0.07, -width * 0.4, 0.0);
		glVertex3f(width * 0.07, -width * 0.4, 0.0);
		glVertex3f(width * 0.07, -width * 0.25, 0.0);
		glEnd();

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.09, -width * 0.25, 0.0);
		glVertex3f(width * 0.09, -width * 0.4, 0.0);
		glVertex3f(width * 0.16, -width * 0.4, 0.0);
		glVertex3f(width * 0.16, -width * 0.25, 0.0);
		glEnd();

		//cuerpo
		drawCircle(0, width * 0.15, width / 6, 50);

		glBegin(GL_POLYGON);
		glVertex3f(width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, -width * 0.25, 0.0);
		glVertex3f(-width * 0.1, width * 0.25, 0.0);
		glVertex3f(width * 0.1, width * 0.25, 0.0);
		glEnd();

		//Base
		glBegin(GL_POLYGON);
		glVertex3f(-width * 0.2, width * 0.25, 0.0);
		glVertex3f(-width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.4, 0.0);
		glVertex3f(width * 0.2, width * 0.25, 0.0);
		glEnd();
	}
}