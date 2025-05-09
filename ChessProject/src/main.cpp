#include "freeglut.h"
#include <iostream>
#include "boardGL.h"
#include "menu.h"


#define CLASSIC 0
#define CLASSIC_KRAMNIK 1
#define DEMI 2
#define DEMI_KRAMNIK 3
#define SILVERBULLET 4
#define SILVERBULLET_KRAMNIK 5

/////////////////////////////////
// Callbacks declarations
void OnDraw(void);
void OnKeyboardDown(unsigned char key, int x, int y);
void OnMouseClick(int button, int state, int x, int y);
void OnTimer(int value);

/////////////////////////////////
// Global objects
Game* game = nullptr;
BoardGL* scene = nullptr;
Menu menu;

/////////////////////////////////
int main(int argc, char* argv[]) {
    // Add menu items
    menu.AddItem("CLASSIC", CLASSIC);
    menu.AddItem("CLASSIC_KRAMNIK", CLASSIC_KRAMNIK);
    menu.AddItem("DEMI", DEMI);
    menu.AddItem("DEMI_KRAMNIK", DEMI_KRAMNIK);
    menu.AddItem("SILVERBULLET", SILVERBULLET);
    menu.AddItem("SILVERBULLET_KRAMNIK", SILVERBULLET_KRAMNIK);

    // GL Initialization
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("MiJuego");

    // Initialize the game and scene
    game = new Game(CLASSIC);
    scene = new BoardGL(*game);

    glutTimerFunc(25, OnTimer, 0);
    glutDisplayFunc(OnDraw);
    glutKeyboardFunc(OnKeyboardDown);
    glutMouseFunc(OnMouseClick);

    initModels();




    // GLUT takes control
    glutMainLoop();
    
    // Free memory when exiting
    delete game;
    delete scene;

    return 0;
}

void OnDraw(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (menu.IsActive()) {
        menu.Draw();
    }
    else {
        scene->Draw();
    }

    glutSwapBuffers();
}

void OnKeyboardDown(unsigned char key, int x_t, int y_t) {
    if (!menu.IsActive()) {
        scene->KeyDown(key);
        glutPostRedisplay();
    }
}

void OnMouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (menu.IsActive()) {
            menu.HandleMouseClick(x, y);
            if (!menu.IsActive()) {
                // Initialize the game with the selected mode
                int selectedMode = menu.GetSelectedMode();
                delete game; // Free previous memory
                delete scene;

                game = new Game(selectedMode);
                scene = new BoardGL(*game);
                scene->init();
            }
        }
        else {
            // Handle clicks on the board
            bool down = (state == GLUT_DOWN);
            int specialKey = glutGetModifiers();
            bool ctrlKey = (specialKey & GLUT_ACTIVE_CTRL) ? true : false;
            bool sKey = specialKey & GLUT_ACTIVE_SHIFT;
            scene->MouseButton(x, y, button, down, sKey, ctrlKey);
        }
    }

    glutPostRedisplay();
}

void OnTimer(int value) {
    if (!menu.IsActive()) {
        scene->OnTimer(value);
    }

    glutTimerFunc(25, OnTimer, 0);
    glutPostRedisplay();
}