#include "window.h"


/////////////////////////////////
// Callbacks declarations
void OnDraw(void);
void OnKeyboardDown(unsigned char key, int x, int y);
void OnMouseClick(int button, int state, int x, int y);
void OnTimer(int value);
void OnSpecialKeyDown(int key, int x, int y);

window* tab = nullptr;

/////////////////////////////////
int main(int argc, char* argv[]) {
    // Add menu items
    tab = new window();

    // GL Initialization
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("Maestros del Ajedrez I");

    tab->setup();

    glutTimerFunc(25,OnTimer, 0);
    glutDisplayFunc(OnDraw);
    glutKeyboardFunc(OnKeyboardDown);
    glutMouseFunc(OnMouseClick);
    glutSpecialFunc(OnSpecialKeyDown); // Añadir esta línea



    // GL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // GLUT takes control
    glutMainLoop();

    return 0;
}

void OnDraw(void) {
    tab->OnDraw();
}

void OnKeyboardDown(unsigned char key, int x_t, int y_t) {
    tab->OnKeyboardDown(key,x_t,y_t);
}

void OnMouseClick(int button, int state, int x, int y) {
    tab->OnMouseClick(button,state,x,y);
}

void OnSpecialKeyDown(int key, int x, int y) {
    tab->OnSpecialKeyDown(key,x,y);
}

void OnTimer(int value) {

    tab->OnTimer(value);
    
    glutTimerFunc(25, OnTimer, 0);
    glutPostRedisplay();
}